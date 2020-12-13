#include "common/runtime.h"
#include "objects/source/source.h"

#include "common/audiopool.h"
#include "modules/audio/audio.h"

#include "audiodriver.h"

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
bool Source::_Update()
{
    for (int which = 0; which < this->buffers; which++)
    {
        if (this->sources[which].state == AudioDriverWaveBufState_Done)
        {
            int decoded = this->StreamAtomic(this->sources[which].data_pcm16, this->decoder.Get());

            if (decoded == 0)
                return false;

            int samples = (int)((decoded / this->channels) / (this->bitDepth / 8));

            this->sources[which].size = decoded;
            this->sources[which].end_sample_offset = samples;

            AudrenDriver::LockFunction([this, which](AudioDriver * driver) {
                audrvVoiceAddWaveBuf(driver, this->channel, &this->sources[which]);
            });
        }
    }

    return true;
}

void Source::CreateWaveBuffer(SoundData * sound)
{
    this->sources[0] = _waveBuf();
    this->sources[0].start_sample_offset = 0;
    this->sources[0].size = sound->GetSize();
    this->sources[0].end_sample_offset = sound->GetSampleCount();
}

void Source::StreamAtomic(size_t which)
{
    this->sources[which].size = decoded;
    this->sources[which].end_sample_offset = (int)((decoded / this->channels) / (this->bitDepth / 8));
}

void Source::FreeBuffer()
{
    if (this->sourceType != TYPE_STATIC)
    {
        for (int i = 0; i < Source::MAX_BUFFERS; i++)
            AudioPool::MemoryFree(std::make_pair(this->sources[i].data_pcm16, this->sources[i].size));
    }
}

void Source::InitializeStreamBuffers(Decoder * decoder)
{
    for (auto & source : this->sources)
    {
        source = AudioDriverWaveBuf();
        source.start_sample_offset = 0;
        source.size = 0;
        source.data_pcm16 = (s16 *)AudioPool::MemoryAlign(decoder->GetSize()).first;
        source.state = AudioDriverWaveBufState_Done
    }
}

void Source::AddWaveBuffer(size_t which)
{
    AudrenDriver::LockFunction([this, which](AudioDriver * driver) {
        audrvVoiceAddWaveBuf(driver, this->channel, &this->sources[which]);
        audrvVoiceStart(driver, this->channel);
    });
}

void Source::Reset()
{
    AudrenDriver::LockFunction([this](AudioDriver * driver) {
        audrvVoiceInit(driver, this->channel, this->channels, PcmFormat_Int16, this->sampleRate);
        audrvVoiceSetDestinationMix(driver, this->channel, AUDREN_FINAL_MIX_ID);

        audrvVoiceSetMixFactor(driver, this->channel, 1.0f, 0, 0);
        audrvVoiceSetMixFactor(driver, this->channel, 1.0f, 0, 1);
    });
}

void Source::ResumeAtomic()
{
    if (this->valid && !this->IsPlaying())
    {
        AudrenDriver::LockFunction([this](AudioDriver * driver) {
            audrvVoiceSetPaused(driver, this->channel, false);
        });
    }
}

bool Source::IsPlaying() const
{
    if (!this->valid)
        return false;

    bool playing = false;

    AudrenDriver::LockFunction([this, &playing](AudioDriver * driver) {
        playing = (audrvVoiceIsPlaying(driver, this->channel) == true);
    });

    return playing;
}

bool Source::IsFinished() const
{
    if (!this->valid)
        return false;

    if (this->sourceType == TYPE_STREAM && (!this->decoder->IsFinished()))
        return false;

    bool finished = false;

    AudrenDriver::LockFunction([this, &finished](AudioDriver * driver) {
        finished = (audrvVoiceIsPlaying(driver, this->channel) == false);
    });

    return finished;
}

void Source::StopAtomic()
{
    AudrenDriver::LockFunction([this](AudioDriver * driver) {
        if (!this->valid)
            return;

        this->TeardownAtomic();
        audrvVoiceStop(driver, this->channel);
    });
}

void Source::PauseAtomic()
{
    AudrenDriver::LockFunction([this](AudioDriver * driver) {
        if (this->valid)
            audrvVoiceSetPaused(driver, this->channel, true);
    });
}

void Source::SetVolume(float volume)
{
    AudrenDriver::LockFunction([this, volume](AudioDriver * driver) {
        audrvVoiceSetVolume(driver, this->channel, volume);
    });

    this->volume = volume;
}
