#include "modules/timer/timer.h"

using namespace love;

static TickCounter counter;

Timer::Timer()
{
    osTickCounterStart(&counter);
    this->prevFPSUpdate = currentTime = this->GetTime();
}

double common::Timer::GetTime()
{
    counter.elapsed = svcGetSystemTick() - counter.reference;
    return osTickCounterRead(&counter) / 1000.0;
}