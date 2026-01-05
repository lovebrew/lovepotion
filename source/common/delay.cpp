#include "common/delay.hpp"

#if defined(__3DS__)
    #include <3ds.h>
#elif defined(__SWITCH__)
    #include <switch.h>
#else
    #include <coreinit/thread.h>
    #include <coreinit/time.h>
#endif

#include <chrono>

void love::sleep(double ms)
{
#if !defined(__WIIU__)
    using namespace std::chrono;

    auto time = duration<double, std::milli>(ms);
    auto ns   = duration_cast<nanoseconds>(time);
    svcSleepThread((int64_t)ns.count());
#else
    OSSleepTicks(OSMillisecondsToTicks(ms));
#endif
}
