#include "modules/timer/timerc.h"

using namespace love::common;

// Löve2D Functions

static constexpr auto SLEEP_DURATION = 1000000ULL;

#if defined(__3DS__)
    #include <3ds.h>
#elif defined(__SWITCH__)
    #include <switch.h>
#endif

uint64_t Timer::reference = 0;

Timer::Timer() :
    currentTime(0),
    prevFPSUpdate(0),
    fps(0),
    averageDelta(0),
    fpsUpdateFrequency(1),
    frames(0),
    dt(0)
{}

double Timer::GetAverageDelta()
{
    return this->averageDelta;
}

double Timer::GetDelta()
{
    return this->dt;
}

int Timer::GetFPS()
{
    return this->fps;
}

/*
** According to LÖVE docs:
** "sleep in seconds"
*/
void Timer::Sleep(float seconds)
{
    if (seconds >= 0)
    {
        u32 milliseconds = seconds * 1000.0f;
        u64 nanoSeconds  = milliseconds * SLEEP_DURATION;

        svcSleepThread(nanoSeconds);
    }
}

double Timer::Step()
{
    this->frames++;

    this->lastTime    = this->currentTime;
    this->currentTime = Timer::GetTime();

    this->dt = this->currentTime - this->lastTime;

    if (this->dt < 0)
        this->dt = 0;

    double timeSinceLast = (this->currentTime - this->prevFPSUpdate);

    if (timeSinceLast > this->fpsUpdateFrequency)
    {
        this->fps           = int((this->frames / timeSinceLast) + 0.5);
        this->averageDelta  = timeSinceLast / frames;
        this->prevFPSUpdate = this->currentTime;
        this->frames        = 0;
    }

    return this->dt;
}
