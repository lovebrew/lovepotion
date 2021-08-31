#include "objects/source/source.h"
#include "modules/audio/audio.h"

#include "driver/audiodrv.h"
#include "pools/audiopool.h"

using namespace love;

#define AudioModule() (Module::GetInstance<Audio>(Module::M_AUDIO))

StaticDataBuffer::StaticDataBuffer(void* data, size_t size)
{
    std::pair<void*, size_t> buff = AudioPool::MemoryAlign(size);

    if (buff.first)
    {
        memcpy(buff.first, data, size);

        this->buffer = { (s16*)buff.first, buff.second };
    }
}

StaticDataBuffer::~StaticDataBuffer()
{
    AudioPool::MemoryFree(this->buffer);
}

/* SOURCE IMPLEMENTATION */

Source::Source(Pool* pool, SoundData* sound) : common::Source(pool, sound)
{
    this->sources[0]                     = AudioDriverWaveBuf();
    this->sources[0].start_sample_offset = 0;
    this->sources[0].size                = sound->GetSize();
    this->sources[0].end_sample_offset   = sound->GetSampleCount();
}

Source::Source(Pool* pool, Decoder* decoder) : common::Source(pool, decoder)
{
    this->InitializeStreamBuffers(decoder);
}

Source::Source(const Source& other) : common::Source(other)
{
    this->InitializeStreamBuffers(this->decoder.Get());
}

love::Source* Source::Clone()
{
    return new Source(*this);
}

Source::~Source()
{
    this->Stop();
    this->FreeBuffer();
}

void Source::InitializeStreamBuffers(Decoder* decoder)
{
    if (!this->sourceBuffer)
    {
        auto aligned = AudioPool::MemoryAlign(decoder->GetSize() * 2);

        this->sourceBuffer    = aligned.first;
        this->souceBufferSize = aligned.second;
    }

    for (size_t i = 0; i < MAX_BUFFERS; i++)
    {
        auto buffer = (s16*)(((size_t)this->sourceBuffer) + i * decoder->GetSize());

        this->sources[i] = AudioDriverWaveBuf { .data_pcm16          = buffer,
                                                .size                = 0,
                                                .start_sample_offset = 0,
                                                .state = AudioDriverWaveBufState_Done };
    }
}

void Source::FreeBuffer()
{
    if (this->sourceType != TYPE_STATIC)
        AudioPool::MemoryFree(std::make_pair(this->sourceBuffer, this->souceBufferSize));
}

double Source::GetSampleOffset()
{
    return driver::Audrv::Instance().GetSampleOffset(this->channel);
}

void Source::SetVolume(float volume)
{
    driver::Audrv::Instance().SetChannelVolume(this->channel, volume);
    this->volume = volume;
}

void Source::Reset()
{
    PcmFormat format = (this->bitDepth == 8) ? PcmFormat_Int8 : PcmFormat_Int16;
    driver::Audrv::Instance().ResetChannel(this->channel, this->channels, format, this->sampleRate);

    this->SetVolume(this->GetVolume());
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
                if (this->sources[which].state == AudioDriverWaveBufState_Done)
                {
                    int decoded = this->StreamAtomic(which);

                    if (decoded == 0)
                        return false;

                    driver::Audrv::Instance().AddWaveBuf(this->channel, &this->sources[which]);
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
            this->sources[0].data_pcm16 = (s16*)this->staticBuffer.Get()->GetBuffer();
            armDCacheFlush(this->sources[0].data_pcm16, this->staticBuffer->GetSize());
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
        armDCacheFlush(buffer, decoded);

    if (this->decoder->IsFinished() && this->IsLooping())
        this->decoder->Rewind();

    this->sources[which].size = decoded;
    this->sources[which].end_sample_offset =
        (int)((decoded / this->channels) / (this->bitDepth / 8));

    return decoded;
}

/* IS IT DOING STUFF */

bool Source::IsPlaying() const
{
    return this->valid && !driver::Audrv::Instance().IsChannelPaused(this->channel);
}

bool Source::IsFinished() const
{
    if (!this->valid)
        return false;

    if (this->sourceType == TYPE_STREAM && (this->IsLooping() || !this->decoder->IsFinished()))
        return false;

    if (this->sourceType == TYPE_STATIC)
        return this->sources[0].state == AudioDriverWaveBufState_Done;

    return driver::Audrv::Instance().IsChannelPlaying(this->channel) == false;
}

/* ATOMIC STATES */

bool Source::PlayAtomic()
{
    this->PrepareAtomic();

    /* add the initial wavebuffer */
    driver::Audrv::Instance().AddWaveBuf(this->channel, &this->sources[0]);

    if (this->sourceType != TYPE_STREAM)
        this->offsetSamples = 0;

    if (this->sourceType == TYPE_STREAM)
        this->valid = true;

    return true;
}

void Source::PauseAtomic()
{
    if (this->valid)
        driver::Audrv::Instance().PauseChannel(this->channel, true);
}

void Source::ResumeAtomic()
{
    if (this->valid && !this->IsPlaying())
        driver::Audrv::Instance().PauseChannel(this->channel, false);
}

void Source::ClearChannel()
{
    driver::Audrv::Instance().StopChannel(this->channel);
}

void Source::StopAtomic()
{
    if (!this->valid)
        return;

    this->TeardownAtomic();
}

void Source::SetLooping(bool should)
{
    this->looping = should;
    if (this->valid && this->sourceType == TYPE_STATIC)
        this->sources[0].is_looping = should;
}
