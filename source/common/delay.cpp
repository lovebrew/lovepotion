#if defined(__3DS__)
    #include <3ds.h>
#elif defined(__SWITCH__)
    #include <switch.h>
#endif

#include "common/delay.h"
#include "modules/timer/timerc.h"

void love::Sleep(float seconds)
{
    u32 milliseconds = seconds * 1000.0f;
    u64 nanoSeconds  = milliseconds * common::Timer::SLEEP_DURATION;

    svcSleepThread(nanoSeconds);
}
