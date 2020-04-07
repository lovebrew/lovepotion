#include "common/runtime.h"
#include "modules/audio/audio.h"

#include "common/exception.h"
#include "modules/audio/pool/pool.h"

using namespace love;

std::atomic<bool> THREAD_RUN = false;
void AudioThreadRunner(void * arg)
{
    Pool * pool = (Pool *)arg;

    while (THREAD_RUN)
    {
        pool->Update();
        svcSleepThread(5000000);
    }
}

Audio::Audio()
{
    if (!R_SUCCEEDED(ndspInit()))
        throw love::Exception("Failed to load ndsp. Please make sure your dspfirm.cdc has been dumped properly.");

    try
    {
        this->pool = new Pool();
    }
    catch (love::Exception &)
    {
        throw;
    }

    THREAD_RUN = true;

    s32 priority = 0;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
    this->poolThread = threadCreate(AudioThreadRunner, this->pool, 0x8000, priority - 1, 0, false);
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
