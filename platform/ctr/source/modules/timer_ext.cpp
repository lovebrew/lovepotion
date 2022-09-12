#include <3ds.h>

#include <modules/timer_ext.hpp>

using namespace love;

TickCounter Timer<Console::CTR>::counter {};

Timer<Console::CTR>::Timer()
{
    osTickCounterStart(&counter);
    this->previousFpsUpdate = this->currentTime = Timer::GetTime();
}

double Timer<Console::CTR>::GetTime()
{
    counter.elapsed = svcGetSystemTick() - counter.reference;
    return osTickCounterRead(&counter) / 1000.0;
}

void Timer<Console::CTR>::Sleep(double seconds) const
{
    if (seconds >= 0)
    {
        auto ms          = seconds * 1000.0f;
        auto nanoseconds = ms * 1000000ULL;

        svcSleepThread(nanoseconds);
    }
}

double Timer<Console::CTR>::Step()
{
    this->frames++;

    this->previousTime = this->currentTime;
    this->currentTime  = Timer::GetTime();

    this->delta = this->currentTime - this->previousTime;

    if (this->delta < 0)
        this->delta = 0;

    double timeSinceLast = (this->currentTime - this->previousFpsUpdate);

    if (timeSinceLast > this->fpsUpdateFrequency)
    {
        this->fps               = int((this->frames / timeSinceLast) + 0.5);
        this->averageDelta      = timeSinceLast / frames;
        this->previousFpsUpdate = this->currentTime;
        this->frames            = 0;
    }

    return this->delta;
}
