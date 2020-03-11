#include "common/runtime.h"
#include "modules/timer/timer.h"

using namespace love;

//Löve2D Functions

#define SLEEP_ULL 1000000ULL

Timer::Timer() : currentTime(0),
                 prevFPSUpdate(0),
                 fps(0),
                 averageDelta(0),
                 fpsUpdateFrequency(1),
                 frames(0),
                 dt(0)
{
    #if defined (_3DS)
        osTickCounterStart(&this->counter);
    #elif defined (__SWITCH__)
        this->reference = armGetSystemTick();
    #endif

    this->prevFPSUpdate = currentTime = this->GetTime();
}

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
** "getTime should return seconds"
** osTickCounterRead returns ms, so divide by 1000
*/
double Timer::GetTime()
{
    #if defined (_3DS)
        this->counter.elapsed = svcGetSystemTick() - this->counter.reference;
        return  osTickCounterRead(&this->counter) / 1000.0;
    #elif defined (__SWITCH__)
        return armTicksToNs(armGetSystemTick() - reference) / 1000000000.0;
    #endif
}

/*
** According to LÖVE docs:
** "sleep in seconds"
*/
void Timer::Sleep(float seconds)
{
    u32 milliseconds = seconds * 1000.0f;
    u64 nanoSeconds = milliseconds * SLEEP_ULL;

    svcSleepThread(nanoSeconds);
}

double Timer::Step()
{
    this->frames++;

    this->lastTime = this->currentTime;
    this->currentTime = this->GetTime();

    this->dt = this->currentTime - this->lastTime;

    if (this->dt < 0)
        this->dt = 0;

    double timeSinceLast = (this->currentTime - this->prevFPSUpdate);

    if (timeSinceLast > this->fpsUpdateFrequency)
    {
        this->fps = int((this->frames / timeSinceLast) + 0.5);
        this->averageDelta = timeSinceLast / frames;
        this->prevFPSUpdate = this->currentTime;
        this->frames = 0;
    }

    return this->dt;
}
