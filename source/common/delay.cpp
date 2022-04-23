#if defined(__3DS__)
    #include <3ds.h>
#elif defined(__SWITCH__)
    #include <switch.h>
#endif

#include "common/delay.h"
#include "modules/timer/timerc.h"

void love::Sleep(float ms)
{
    uint32_t milliseconds = static_cast<uint32_t>(ms);
    uint64_t nanoSeconds  = milliseconds * common::Timer::SLEEP_DURATION;

    svcSleepThread(nanoSeconds);
}
