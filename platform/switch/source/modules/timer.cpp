#include "modules/timer/timer.h"
#include <switch.h>

using namespace love;

static u64 reference = (u64)0;

#define NS_TO_SEC 1000000000.0

Timer::Timer()
{
    reference = armGetSystemTick();
    this->prevFPSUpdate = currentTime = this->GetTime();
}

double common::Timer::GetTime()
{
    auto nanoseconds = armTicksToNs(armGetSystemTick() - reference);
    return  nanoseconds / NS_TO_SEC;
}