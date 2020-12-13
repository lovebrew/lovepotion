#include "common/runtime.h"
#include "modules/audio/audio.h"
#include "modules/audio/pool/pool.h"

#include "audiodriver.h"

using namespace love;

/* POOL THREAD */

Audio::PoolThread::PoolThread(Pool * pool) : pool(pool),
                                             finish(false)
{
    this->threadName = "AudioPool";
}

Audio::PoolThread::~PoolThread()
{}

void Audio::PoolThread::ThreadFunction()
{
    while (true)
    {
        if (this->finish)
            return;

        this->pool->Update();
        this->pool->Sleep();
    }
}

void Audio::PoolThread::SetFinish()
{
    this->finish = true;
}

/* POOL THREAD */

Audio::Audio() : pool(nullptr),
                 poolThread(nullptr)
{
    try
    {
        this->pool = new Pool();
    }
    catch (love::Exception &)
    {
        throw;
    }

    AudrenDriver::Initialize();

    this->poolThread = new PoolThread(pool);
    this->poolThread->Start();
}

Audio::~Audio()
{
    this->poolThread->SetFinish();
    this->poolThread->Wait();

    delete this->poolThread;
    delete this->pool;

    AudrenDriver::Exit();
}

float Audio::GetVolume() const
{
    return this->volume;
}

Source * Audio::NewSource(Decoder * decoder)
{
    return new Source(this->pool, decoder);
}

Source * Audio::NewSource(SoundData * sound)
{
    return new Source(this->pool, sound);
}

bool Audio::Play(Source * source)
{
    return source->Play();
}

void Audio::Stop(Source * source)
{
    source->Stop();
}

void Audio::Pause(Source * source)
{
    source->Pause();
}

void Audio::Pause()
{}

void Audio::Stop()
{

}
