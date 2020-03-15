#include "common/runtime.h"
#include "modules/audio/audio.h"

#include "common/exception.h"

using namespace love;

void Audio::UpdateAudioDriver()
{}

void Audio::AddSourceToPool(Source * s)
{
    this->pool.push_back(s);
}

void Threadfunction(void * arg)
{
    std::vector<Source *> * pool = (std::vector<Source *> *)arg;

    while (true)
    {
        for (auto item : *pool)
            item->Update();

        svcSleepThread(5000000);
    }
}

Audio::Audio()
{
    if (!R_SUCCEEDED(ndspInit()))
        throw love::Exception("Failed to load ndsp. Please make sure your dspfirm.cdc has been dumped properly.");

    Thread thread;
    s32 priority = 0;

    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);

    thread = threadCreate(Threadfunction, (void *)&this->pool, 0x8000, priority - 1, 0, false);
}

Audio::~Audio()
{
    ndspExit();
}

void Audio::SetVolume(float volume)
{
    ndspSetMasterVol(volume);
    this->volume = volume;
}
