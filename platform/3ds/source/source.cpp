#include "common/runtime.h"
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

int Source::GetLastIndex()
{
    int which = 0;
    if (this->sources[0].status != NDSP_WBUF_DONE)
    {
        if (this->sources[1].status == NDSP_WBUF_DONE)
            which = 1;
    }

    return which;
}

bool Source::_Update()
{
    for (int which = 0; which < this->buffers; which++)
    {
        if (this->sources[which].status == NDSP_WBUF_DONE)
        {
            int decoded = this->StreamAtomic(this->sources[which].data_pcm16, this->decoder.Get());

            if (decoded == 0)
                return false;

            this->_Prepare(which, decoded);
            ndspChnWaveBufAdd(this->channel, &this->sources[which]);
        }
    }

    return true;
}

void Source::_Prepare(size_t which, size_t decoded)
{
    if (which >= 0)
        this->sources[which].nsamples = (int)((decoded / this->channels) / (this->bitDepth / 8));
}

void Source::FreeBuffer()
{
    if (this->sourceType != TYPE_STATIC)
    {
        for (int i = 0; i < this->buffers; i++)
            linearFree(this->sources[i].data_pcm16);
    }
}

void Source::CreateWaveBuffer(SoundData * sound)
{
    this->sources[0] = _waveBuf();
    this->sources[0].nsamples = sound->GetSampleCount();
}

void Source::CreateWaveBuffer(Decoder * decoder)
{
    for (int i = 0; i < 2; i++)
    {
        this->sources[i] = _waveBuf();
        this->sources[i].nsamples = 0;
        this->sources[i].data_pcm16 = (s16 *)linearAlloc(decoder->GetSize());
        this->sources[i].status = NDSP_WBUF_DONE;
    }
}

void Source::AddWaveBuffer(size_t which)
{
    if (which >= 0)
        ndspChnWaveBufAdd(this->channel, &this->sources[which]);
}

void Source::SetVolume(float volume)
{
    this->volume = volume;

    float mix[12];
    memset(mix, 0, sizeof(mix));

    mix[0] = mix[1] = volume;

    ndspChnSetMix(this->channel, mix);
}

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

void Source::ResumeAtomic()
{
    if (this->valid && !this->IsPlaying())
        ndspChnSetPaused(this->channel, false);
}

void Source::PauseAtomic()
{
    if (this->valid)
        ndspChnSetPaused(this->channel, true);
}

void Source::StopAtomic()
{
    if (!this->valid)
        return;

    this->TeardownAtomic();
    ndspChnWaveBufClear(this->channel);
}

bool Source::IsPlaying() const
{
    if (!this->valid)
        return false;

    return ndspChnIsPlaying(this->channel) == true;
}

bool Source::IsFinished() const
{
    if (!this->valid)
        return false;

    if (this->sourceType == TYPE_STREAM && (!this->decoder->IsFinished()))
        return false;

    return ndspChnIsPlaying(this->channel) == false;
}
