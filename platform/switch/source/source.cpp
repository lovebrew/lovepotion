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
    // for (int i = 0; i < 2; i++)
    // {
    //     this->sources[i] = waveBuffer();
    //     this->sources[i].start_sample_offset = 0;
    //     this->sources[i].data_pcm16 = (s16 *)AudioPool::MemoryAlign(decoder->GetSize());
    // }
    // this->sources[1].status = NDSP_WBUF_DONE;
}

void Source::AddWaveBuffer()
{
    armDCacheFlush(this->sources[0].data_pcm16, this->staticBuffer->GetSize());

    audrvVoiceStop(&AUDIO_DRIVER, this->channel);
    audrvVoiceAddWaveBuf(&AUDIO_DRIVER, this->channel, &this->sources[0]);
    audrvVoiceStart(&AUDIO_DRIVER, this->channel);
}

void Source::Reset()
{}

void Source::ResumeAtomic()
{
    if (this->valid && !this->IsPlaying())
        audrvVoiceSetPaused(&AUDIO_DRIVER, this->channel, false);
}

bool Source::Play()
{
    if (AUDIO_DRIVER.in_voices[this->channel].state != AudioRendererVoicePlayState_Paused)
        return this->valid = this->PlayAtomic();

    this->ResumeAtomic();

    return this->valid = true;
}

bool Source::IsPlaying() const
{
    if (!this->valid)
        return false;

    return (audrvVoiceIsPlaying(&AUDIO_DRIVER, this->channel) == true);
}

void Source::Stop()
{
    if (!this->valid)
        return;

    audrvVoiceStop(&AUDIO_DRIVER, this->channel);
}

bool Source::IsFinished() const
{
    if (!this->valid)
        return false;

    if (this->sourceType == TYPE_STREAM && (!this->decoder->IsFinished()))
        return false;

    return (audrvVoiceIsPlaying(&AUDIO_DRIVER, this->channel) == false);
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
