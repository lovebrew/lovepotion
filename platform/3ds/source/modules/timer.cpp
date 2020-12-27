#include "modules/timer/timer.h"

using namespace love;

static TickCounter counter = { 0 };

Timer::Timer()
{
    osTickCounterStart(&counter);
}

double common::Timer::GetTime()
{
    counter.elapsed = svcGetSystemTick() - counter.reference;
    return osTickCounterRead(&counter) / 1000.0;
}