#include <modules/audio/audio.hpp>

using namespace love;

#include <utilities/log/logfile.h>

Audio::Audio()
{
    DSP<Console::Which>::Instance().Initialize();

    try
    {
        this->pool = new AudioPool();
    }
    catch (love::Exception&)
    {
        throw;
    }

    this->thread = new PoolThread(this->pool);
    bool started = this->thread->Start();
    LOG("Audio Thread: %d", started);
}

Audio::~Audio()
{
    this->thread->SetFinish();
    this->thread->Wait();

    delete this->thread;
    delete this->pool;
}

Source<Console::Which>* Audio::NewSource(Decoder* decoder) const
{
    return new Source<Console::Which>(this->pool, decoder);
}

Source<Console::Which>* Audio::NewSource(SoundData* soundData) const
{
    return new Source<Console::Which>(this->pool, soundData);
}

Source<Console::Which>* Audio::NewSource(int sampleRate, int bitDepth, int channels,
                                         int buffers) const
{
    return new Source<Console::Which>(this->pool, sampleRate, bitDepth, channels, buffers);
}

void Audio::SetVolume(float volume)
{
    DSP<Console::Which>::Instance().SetMasterVolume(volume);
}

float Audio::GetVolume() const
{
    return DSP<Console::Which>::Instance().GetMasterVolume();
}

int Audio::GetActiveSourceCount() const
{
    return this->pool->GetActiveSourceCount();
}

int Audio::GetMaxSources() const
{
    return this->pool->GetMaxSources();
}

bool Audio::Play(Source<Console::Which>* source)
{
    return source->Play();
}

bool Audio::Play(const std::vector<Source<Console::Which>*>& sources)
{
    return Source<Console::Which>::Play(sources);
}

void Audio::Stop(Source<Console::Which>* source)
{
    source->Stop();
}

void Audio::Stop(const std::vector<Source<Console::Which>*>& sources)
{
    Source<Console::Which>::Stop(sources);
}

void Audio::Stop()
{
    Source<Console::Which>::Stop(this->pool);
}

void Audio::Pause(Source<Console::Which>* source)
{
    source->Pause();
}

void Audio::Pause(const std::vector<Source<Console::Which>*>& sources)
{
    Source<Console::Which>::Pause(sources);
}

std::vector<Source<Console::Which>*> Audio::Pause()
{
    return Source<Console::Which>::Pause(this->pool);
}
