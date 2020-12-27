#include "objects/source/source.h"
#include "modules/audio/audio.h"

#include "driver/audrv.h"

using namespace love;

#define AudioModule() (Module::GetInstance<Audio>(Module::M_AUDIO))

#define AUDIO_DRIVER  g_AudioDriver
#define AUDIO_MUTEX   g_audrvMutex

StaticDataBuffer::StaticDataBuffer(void * data, size_t size)
{
    std::pair<void *, size_t> buff = AudioPool::MemoryAlign(size);

    if (buff.first)
    {
        memcpy(buff.first, data, size);

        this->buffer = {(s16 *)buff.first, buff.second};
    }
}

StaticDataBuffer::~StaticDataBuffer()
{
    AudioPool::MemoryFree(this->buffer);
}

/* SOURCE IMPLEMENTATION */

Source::Source(Pool * pool, SoundData * sound) : common::Source(pool, sound)
{
    this->sources[0] = AudioDriverWaveBuf();
    this->sources[0].start_sample_offset = 0;
    this->sources[0].size = sound->GetSize();
    this->sources[0].end_sample_offset = sound->GetSampleCount();
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
        source = AudioDriverWaveBuf();
        source.start_sample_offset = 0;
        source.size = 0;
        source.data_pcm16 = (s16 *)AudioPool::MemoryAlign(decoder->GetSize()).first;
        source.state = AudioDriverWaveBufState_Done;
    }
}

void Source::FreeBuffer()
{
    if (this->sourceType != TYPE_STATIC)
    {
        for (int i = 0; i < Source::MAX_BUFFERS; i++)
            AudioPool::MemoryFree(std::make_pair(this->sources[i].data_pcm16, this->sources[i].size));
    }
}

void Source::SetVolume(float volume)
{
    audrv.LockFunction([this, volume](AudioDriver * driver) {
        audrvVoiceSetVolume(driver, this->channel, volume);
    });

    this->volume = volume;
}

void Source::Reset()
{
    audrv.LockFunction([this](AudioDriver * driver) {
        audrvVoiceInit(driver, this->channel, this->channels, PcmFormat_Int16, this->sampleRate);
        audrvVoiceSetDestinationMix(driver, this->channel, AUDREN_FINAL_MIX_ID);

        audrvVoiceSetMixFactor(driver, this->channel, 1.0f, 0, 0);
        audrvVoiceSetMixFactor(driver, this->channel, 1.0f, 0, 1);
    });
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

                    audrv.LockFunction([this, which](AudioDriver * driver) {
                        audrvVoiceAddWaveBuf(driver, this->channel, &this->sources[which]);
                        audrvVoiceStart(driver, this->channel);
                    });
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
        armDCacheFlush(buffer, decoded);

    if (this->decoder->IsFinished() && this->IsLooping())
        this->decoder->Rewind();

    this->sources[which].size = decoded;
    this->sources[which].end_sample_offset = (int)((decoded / this->channels) / (this->bitDepth / 8));

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
        sourcePlaying |= (this->sources[index].state == AudioDriverWaveBufState_Queued || this->sources[index].state == AudioDriverWaveBufState_Playing);

    return sourcePlaying;
}

bool Source::IsFinished() const
{
    if (!this->valid)
        return false;

    if (this->sourceType == TYPE_STREAM && (!this->decoder->IsFinished()))
        return false;

    bool finished = false;

    audrv.LockFunction([this, &finished](AudioDriver * driver) {
        finished = (audrvVoiceIsPlaying(driver, this->channel) == false);
    });

    return finished;
}

/* ATOMIC STATES */

bool Source::PlayAtomic()
{
    this->PrepareAtomic();

    bool success = false;

    if (this->sourceType != TYPE_STREAM) /* flush the DSP data cache */
        armDCacheFlush(this->sources[0].data_pcm16, this->staticBuffer->GetSize());

    /* add the initial wavebuffer */
    audrv.LockFunction([this](AudioDriver * driver) {
        audrvVoiceAddWaveBuf(driver, this->channel, &this->sources[0]);
        audrvVoiceStart(driver, this->channel);
    });

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
    audrv.LockFunction([this](AudioDriver * driver) {
        if (this->valid)
            audrvVoiceSetPaused(driver, this->channel, true);
    });
}

void Source::ResumeAtomic()
{
    if (this->valid && !this->IsPlaying())
    {
        audrv.LockFunction([this](AudioDriver * driver) {
            audrvVoiceSetPaused(driver, this->channel, false);
        });
    }
}

void Source::StopAtomic()
{
    audrv.LockFunction([this](AudioDriver * driver) {
        if (!this->valid)
            return;

        this->TeardownAtomic();
        audrvVoiceStop(driver, this->channel);
    });
}

void Source::SetLooping(bool should)
{
    this->looping = should;
    if (this->valid && this->sourceType == TYPE_STATIC)
        this->sources[0].is_looping = should;
}
