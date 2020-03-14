#include "common/runtime.h"
#include "objects/source/source.h"

#include "common/pool.h"
#include "modules/audio/audio.h"

using namespace love;

#define AudioModule() (Module::GetInstance<Audio>(Module::M_AUDIO))
#define AUDIO_DRIVER  AudioModule()->GetAudioDriver()

StaticDataBuffer::StaticDataBuffer(void * data, size_t size)
{
    std::pair<void *, size_t> buff = AudioPool::MemoryAlign(size);
    memcpy(buff.first, data, size);

    this->buffer = {(s16 *)buff.first, buff.second};
}

StaticDataBuffer::~StaticDataBuffer()
{
    AudioPool::MemoryFree(this->buffer);
}

waveBuffer Source::CreateWaveBuffer(size_t size, size_t nsamples)
{
    waveBuffer buffer = waveBuffer();

    buffer.size = size;
    buffer.start_sample_offset = this->offsetSamples;
    buffer.end_sample_offset = nsamples;

    return buffer;
}

void Source::AddWaveBuffer()
{
    armDCacheFlush(this->source.data_pcm16, this->staticBuffer->GetSize());

    audrvVoiceStop(&AUDIO_DRIVER, 0);
    audrvVoiceAddWaveBuf(&AUDIO_DRIVER, 0, &this->source);
    audrvVoiceStart(&AUDIO_DRIVER, 0);
}

void Source::Reset()
{}

void Source::ResumeAtomic()
{
    if (this->valid && !this->IsPlaying())
        audrvVoiceSetPaused(&AUDIO_DRIVER, 0, false);
}

bool Source::Play()
{
    if (AUDIO_DRIVER.in_voices[0].state != AudioRendererVoicePlayState_Paused)
        return this->valid = this->PlayAtomic();

    this->ResumeAtomic();

    return this->valid = true;
}

bool Source::IsPlaying() const
{
    if (!this->valid)
        return false;

    return (audrvVoiceIsPlaying(&AUDIO_DRIVER, 0) == true);
}

void Source::Stop()
{
    if (!this->valid)
        return;

    audrvVoiceStop(&AUDIO_DRIVER, 0);
}

bool Source::IsFinished() const
{
    if (!this->valid)
        return false;

    if (this->sourceType == TYPE_STREAM && (!this->decoder->IsFinished()))
        return false;

    return (audrvVoiceIsPlaying(&AUDIO_DRIVER, 0) == false);
}

void Source::Pause()
{}
