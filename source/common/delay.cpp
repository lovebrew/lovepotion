#include "common/delay.hpp"

#if defined(__3DS__)
    #include <3ds.h>
#elif defined(__SWITCH__)
    #include <switch.h>
#else
    #include <coreinit/thread.h>
    #include <coreinit/time.h>
#endif

void love::sleep(double ms)
{
#if !defined(__WIIU__)
    svcSleepThread(NS_TO_MS * ms);
#else
    OSSleepTicks(OSMillisecondsToTicks(ms));
#endif
}
