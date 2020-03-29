#include "common/runtime.h"
#include "modules/audio/audio.h"

#include "common/exception.h"

using namespace love;

void Audio::UpdateAudioDriver()
{}

std::atomic<bool> THREAD_RUN;

void Threadfunction(void * arg)
{
    auto pool = (std::vector<Source *> *)arg;

    while (THREAD_RUN)
    {
        auto iterator = pool->begin();

        while (iterator != pool->end())
        {
            if (!(*iterator)->Update())
            {
                (*iterator)->Release();
                iterator = pool->erase(iterator);
            }
            else
                iterator++;
        }

        /* Sleep for 5ms */
        svcSleepThread(5000000);
    }
}

Audio::Audio()
{
    if (!R_SUCCEEDED(ndspInit()))
        throw love::Exception("Failed to load ndsp. Please make sure your dspfirm.cdc has been dumped properly.");

    s32 priority = 0;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);

    THREAD_RUN = true;
    this->poolThread = threadCreate(Threadfunction, (void *)&this->pool, 0x8000, priority - 1, 0, false);
}

Audio::~Audio()
{
    THREAD_RUN = false;

    threadJoin(this->poolThread, U64_MAX);
    threadFree(this->poolThread);

    ndspExit();
}

void Audio::SetVolume(float volume)
{
    ndspSetMasterVol(volume);
    this->volume = volume;
}
