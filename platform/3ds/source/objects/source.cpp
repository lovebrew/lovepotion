#include "objects/source/source.h"

using namespace love;

StaticDataBuffer::StaticDataBuffer(void * data, size_t size)
{
    this->buffer.first = (s16 *)linearAlloc(size);
    this->buffer.second = size;

    memcpy(this->buffer.first, data, size);
}

StaticDataBuffer::~StaticDataBuffer()
{
    linearFree(this->buffer.first);
}

/* SOURCE IMPLEMENTATION */

Source::Source(Pool * pool, SoundData * sound) : common::Source(pool, sound)
{
    this->sources[0] = ndspWaveBuf();
    this->sources[0].nsamples = sound->GetSampleCount();
}

Source::Source(Pool * pool, Decoder * decoder) : common::Source(pool, decoder)
{
    this->InitializeStreamBuffers(decoder);
}

love::Source * Source::Clone()
{
    return new Source(*this);
}

Source::~Source()
{
    this->Stop();
    this->FreeBuffer();
}

void Source::InitializeStreamBuffers(Decoder * decoder)
{
    for (auto & source : this->sources)
    {
        source = ndspWaveBuf
        {
            .data_pcm16 = (s16 *)linearAlloc(decoder->GetSize()),
            .nsamples = 0,
            .status = NDSP_WBUF_DONE
        };
    }
}

void Source::FreeBuffer()
{
    if (this->sourceType != TYPE_STATIC)
    {
        for (int i = 0; i < Source::MAX_BUFFERS; i++)
            linearFree(this->sources[i].data_pcm16);
    }
}

void Source::SetVolume(float volume)
{
    this->volume = volume;

    float mix[12];
    memset(mix, 0, sizeof(mix));

    mix[0] = mix[1] = volume;

    ndspChnSetMix(this->channel, mix);
}

/* IMPORTANT STUFF */

void Source::Reset()
{
    ndspChnReset(this->channel);

    u16 format = NDSP_FORMAT_STEREO_PCM16;

    switch (this->channels)
    {
        case 1:
            if (this->bitDepth == 8)
                format = NDSP_FORMAT_MONO_PCM8;
            else
                format = NDSP_FORMAT_MONO_PCM16;

            break;
        case 2:
            if (this->bitDepth == 8)
                format = NDSP_FORMAT_STEREO_PCM8;
            else
                format = NDSP_FORMAT_STEREO_PCM16;

            break;
        default:
            break;
    }

    ndspInterpType interpType = (this->channels == 2) ?
                                    NDSP_INTERP_POLYPHASE : NDSP_INTERP_LINEAR;

    ndspChnSetFormat(this->channel, format);
    ndspChnSetRate(this->channel, this->sampleRate);
    ndspChnSetInterp(this->channel, interpType);
}

bool Source::Update()
{
    if (!this->valid)
        return false;

    switch (this->sourceType)
    {
        case TYPE_STATIC:
            return !this->IsFinished();
        case TYPE_STREAM:
        {
            if (this->IsFinished())
                return false;

            for (int which = 0; which < Source::MAX_BUFFERS; which++)
            {
                if (this->sources[which].status == NDSP_WBUF_DONE)
                {
                    int decoded = this->StreamAtomic(which);

                    if (decoded == 0)
                        return false;

                    ndspChnWaveBufAdd(this->channel, &this->sources[which]);
                }
            }

            return true;
        }
        case TYPE_QUEUE:
            break;
        case TYPE_MAX_ENUM:
        default:
            break;
    }

    return false;
}

void Source::PrepareAtomic()
{
    this->Reset();

    switch (this->sourceType)
    {
        case TYPE_STATIC:
            this->sources[0].data_pcm16 = (s16 *)this->staticBuffer.Get()->GetBuffer();
            break;
        case TYPE_STREAM:
        {
            if (this->StreamAtomic(0) == 0)
                break;

            if (this->decoder->IsFinished())
                break;

            break;
        }
        case TYPE_QUEUE:
        default:
            break;
    }
}

int Source::StreamAtomic(size_t which)
{
    auto buffer = this->sources[which].data_pcm16;
    int decoded = std::max(decoder->Decode(buffer), 0);

    if (decoded > 0)
        DSP_FlushDataCache(buffer, decoded);

    if (this->decoder->IsFinished() && this->IsLooping())
        this->decoder->Rewind();

    this->sources[which].nsamples = (int)((decoded / this->channels) / (this->bitDepth / 8));

    return decoded;
}

/* IS IT DOING STUFF */

bool Source::IsPlaying() const
{
    if (!this->valid)
        return false;

    bool sourcePlaying = false;
    size_t bufferCount = (this->sourceType == TYPE_STREAM) ? MAX_BUFFERS : 1;
    /* check if any of our sources are queued or playing */
    for (size_t index = 0; index < bufferCount; index++)
        sourcePlaying |= (this->sources[index].status == NDSP_WBUF_QUEUED || this->sources[index].status == NDSP_WBUF_PLAYING);

    return sourcePlaying;
}

bool Source::IsFinished() const
{
   if (!this->valid)
        return false;

    if (this->sourceType == TYPE_STREAM && (this->IsLooping() || !this->decoder->IsFinished()))
        return false;

    return ndspChnIsPlaying(this->channel) == false;
}

/* ATOMIC STATES */

bool Source::PlayAtomic()
{
    this->PrepareAtomic();

    bool success = false;

    if (this->sourceType != TYPE_STREAM) /* flush the DSP data cache */
        DSP_FlushDataCache(this->sources[0].data_pcm16, this->staticBuffer->GetSize());

    /* add the initial wavebuffer */
    ndspChnWaveBufAdd(this->channel, &this->sources[0]);

    success = true;

    //isPlaying() needs source to be valid
    if (this->sourceType == TYPE_STREAM)
    {
        this->valid = true;

        if (!this->IsPlaying())
            success = false;
    }

    //stop() needs source to be valid
    if (!success)
    {
        this->valid = true;
        this->Stop();
    }

    if (this->sourceType != TYPE_STREAM)
        this->offsetSamples = 0;

    return success;
}

void Source::PauseAtomic()
{
    if (this->valid)
        ndspChnSetPaused(this->channel, true);
}

void Source::ResumeAtomic()
{
    if (this->valid && !this->IsPlaying())
        ndspChnSetPaused(this->channel, false);
}

void Source::StopAtomic()
{
    if (!this->valid)
        return;

    this->TeardownAtomic();
    ndspChnWaveBufClear(this->channel);
}

void Source::SetLooping(bool should)
{
    this->looping = should;
    if (this->valid && this->sourceType == TYPE_STATIC)
        this->sources[0].looping = should;
}