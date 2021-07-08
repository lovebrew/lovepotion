#include "modules/audio/audio.h"
#include "modules/audio/pool/pool.h"

using namespace love;

/* POOL THREAD */

Audio::PoolThread::PoolThread(Pool* pool) : pool(pool), finish(false)
{
    this->threadName = "AudioPool";
}

Audio::PoolThread::~PoolThread()
{}

void Audio::PoolThread::ThreadFunction()
{
    while (!this->finish)
    {
        this->pool->Update();
        this->pool->Sleep();
    }
}

void Audio::PoolThread::SetFinish()
{
    this->finish = true;
}

/* POOL THREAD */

Audio::Audio() : pool(nullptr), poolThread(nullptr)
{
    if (!driver::Audrv::Instance().IsInitialized())
        throw love::Exception("Failed to open device.");

    try
    {
        this->pool = new Pool();
    }
    catch (love::Exception&)
    {
        throw;
    }

    this->poolThread = new PoolThread(pool);
    this->poolThread->Start();
}

Audio::~Audio()
{
    this->poolThread->SetFinish();
    this->poolThread->Wait();

    delete this->poolThread;
    delete this->pool;
}

float Audio::GetVolume() const
{
    return this->volume;
}

int Audio::GetActiveSourceCount() const
{
    return this->pool->GetActiveSourceCount();
}

int Audio::GetMaxSources() const
{
    return this->pool->GetMaxSources();
}

Source* Audio::NewSource(Decoder* decoder)
{
    return new Source(this->pool, decoder);
}

Source* Audio::NewSource(SoundData* sound)
{
    return new Source(this->pool, sound);
}

bool Audio::Play(const std::vector<common::Source*>& sources)
{
    return common::Source::Play(sources);
}

bool Audio::Play(Source* source)
{
    return source->Play();
}

bool Audio::Play()
{
    return true;
}

void Audio::Stop(const std::vector<common::Source*>& sources)
{
    return common::Source::Stop(sources);
}

void Audio::Stop(Source* source)
{
    source->Stop();
}

void Audio::Stop()
{
    return common::Source::Stop(this->pool);
}

void Audio::Pause(Source* source)
{
    source->Pause();
}

void Audio::Pause(const std::vector<common::Source*>& sources)
{
    return common::Source::Pause(sources);
}

std::vector<common::Source*> Audio::Pause()
{
    return common::Source::Pause(this->pool);
}
