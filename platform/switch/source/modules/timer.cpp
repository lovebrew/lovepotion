#include "modules/timer/timer.h"
#include <switch.h>

using namespace love;

static constexpr auto NS_TO_SEC = 1000000000.0;

Timer::Timer()
{
    Timer::reference    = armGetSystemTick();
    this->prevFPSUpdate = currentTime = this->GetTime();
}

double common::Timer::GetTime()
{
    return armTicksToNs(armGetSystemTick() - Timer::reference) / NS_TO_SEC;
}