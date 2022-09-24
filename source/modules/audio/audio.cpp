#include <modules/audio/audio.hpp>

using namespace love;

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
    this->thread->Start();
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

void Audio::Stop(Source<Console::Which>* source)
{
    source->Stop();
}
