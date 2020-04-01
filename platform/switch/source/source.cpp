#include "common/runtime.h"
#include "objects/source/source.h"

#include "common/pool.h"
#include "modules/audio/audio.h"

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

bool Source::_Update()
{
    int index = 0;
    if (this->sources[index].state != AudioDriverWaveBufState_Done)
    {
        if (this->sources[index + 1].state == AudioDriverWaveBufState_Done)
            index = 1;
    }

    if (this->sources[index].state == AudioDriverWaveBufState_Done)
    {
        int decoded = this->StreamAtomic(this->sources[index].data_pcm16, this->decoder.Get());

        if (decoded == 0)
            return false;

        int samples = (int)((decoded / this->channels) / (this->bitDepth / 8));

        this->sources[index].end_sample_offset = samples;
        this->sources[index].size = decoded;

        mutexLock(&AUDIO_MUTEX);

        audrvVoiceStop(&AUDIO_DRIVER, this->channel);
        audrvVoiceAddWaveBuf(&AUDIO_DRIVER, this->channel, &this->sources[this->index]);
        audrvVoiceStart(&AUDIO_DRIVER, this->channel);

        mutexUnlock(&AUDIO_MUTEX);
    }

    return true;
}

void Source::CreateWaveBuffer(SoundData * sound)
{
    this->sources[0] = waveBuffer();
    this->sources[0].start_sample_offset = 0;
    this->sources[0].size = sound->GetSize();
    this->sources[0].end_sample_offset = sound->GetSampleCount();
}

void Source::_PrepareSamples(int samples)
{
    this->sources[this->index].end_sample_offset = samples;
}

void Source::CreateWaveBuffer(Decoder * decoder)
{
    for (int i = 0; i < 2; i++)
    {
        this->sources[i] = waveBuffer();
        this->sources[i].start_sample_offset = 0;
        this->sources[i].data_pcm16 = (s16 *)AudioPool::MemoryAlign(decoder->GetSize()).first;
    }
    this->sources[1].state = AudioDriverWaveBufState_Done;
}

void Source::AddWaveBuffer()
{
    mutexLock(&AUDIO_MUTEX);

    audrvVoiceStop(&AUDIO_DRIVER, this->channel);
    audrvVoiceAddWaveBuf(&AUDIO_DRIVER, this->channel, &this->sources[this->index]);
    audrvVoiceStart(&AUDIO_DRIVER, this->channel);

    mutexUnlock(&AUDIO_MUTEX);
}

void Source::Reset()
{
    mutexLock(&AUDIO_MUTEX);

    audrvVoiceInit(&AUDIO_DRIVER, this->channel, 1, PcmFormat_Int16, this->sampleRate);
    audrvVoiceSetDestinationMix(&AUDIO_DRIVER, this->channel, AUDREN_FINAL_MIX_ID);

    audrvVoiceSetMixFactor(&AUDIO_DRIVER, this->channel, 1.0f, 0, 0);
    audrvVoiceSetMixFactor(&AUDIO_DRIVER, this->channel, 1.0f, 0, 1);

    audrvVoiceStart(&AUDIO_DRIVER, this->channel);

    mutexUnlock(&AUDIO_MUTEX);
}

void Source::ResumeAtomic()
{
    if (this->valid && !this->IsPlaying())
    {
        mutexLock(&AUDIO_MUTEX);

        audrvVoiceSetPaused(&AUDIO_DRIVER, this->channel, false);

        mutexUnlock(&AUDIO_MUTEX);
    }
}

bool Source::Play()
{
    if (!this->IsPlaying())
        return this->valid = this->PlayAtomic();

    this->ResumeAtomic();

    return this->valid = true;
}

bool Source::IsPlaying() const
{
    if (!this->valid)
        return false;

    mutexLock(&AUDIO_MUTEX);

    bool playing = (audrvVoiceIsPlaying(&AUDIO_DRIVER, this->channel) == true);

    mutexUnlock(&AUDIO_MUTEX);

    return playing;
}

void Source::Stop()
{
    if (!this->valid)
        return;

    mutexLock(&AUDIO_MUTEX);

    audrvVoiceStop(&AUDIO_DRIVER, this->channel);

    mutexUnlock(&AUDIO_MUTEX);
}

bool Source::IsFinished() const
{
    if (!this->valid)
        return false;

    if (this->sourceType == TYPE_STREAM && (!this->decoder->IsFinished()))
        return false;

    mutexLock(&AUDIO_MUTEX);

    bool finished = (audrvVoiceIsPlaying(&AUDIO_DRIVER, this->channel) == false);

    mutexUnlock(&AUDIO_MUTEX);

    return finished;
}

void Source::Pause()
{}

void Source::SetLooping(bool shouldLoop)
{
    this->looping = shouldLoop;

    for (int i = 0; i < this->buffers; i ++)
        this->sources[i].is_looping = shouldLoop;
}

void Source::SetVolume(float volume)
{}

void Source::FreeBuffer()
{}
