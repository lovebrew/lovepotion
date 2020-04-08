#include "common/runtime.h"
#include "modules/audio/audio.h"

#include "audiodriver.h"

using namespace love;

std::atomic<bool> THREAD_RUN;

void Audio::_Initialize()
{
    u32 priority = 0;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
    Result ret = threadCreate(&this->poolThread, AudioThreadRunner, (void *)&this->pool, NULL, 0x10000, priority - 1, 0);

    if (R_SUCCEEDED(ret))
        threadStart(&this->poolThread);
}

void Audio::_Exit()
{
    threadWaitForExit(&this->poolThread);
    threadClose(&this->poolThread);
}

void Audio::SetVolume(float volume)
{
    AudrenDriver::LockFunction([volume](AudioDriver * driver) {
        for (int i = 0; i < 2; i++)
            audrvMixSetVolume(driver, i, volume);
    });
}
