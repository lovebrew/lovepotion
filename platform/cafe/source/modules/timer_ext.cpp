#include <modules/timer_ext.hpp>

#include <coreinit/thread.h>

using namespace love;

OSTick Timer<Console::CAFE>::reference = 0;

Timer<Console::CAFE>::Timer()
{
    Timer::reference        = OSGetSystemTick();
    this->previousFpsUpdate = this->currentTime = Timer::GetTime();
}

double Timer<Console::CAFE>::GetTime()
{
    auto nanoseconds = OSTicksToNanoseconds(OSGetSystemTick() - Timer::reference);
    return nanoseconds / NANOSECONDS_TO_SECONDS;
}

void Timer<Console::CAFE>::Sleep(double seconds) const
{
    if (seconds >= 0)
    {
        auto milliseconds = seconds * MILLISECONDS;
        auto nanoseconds  = milliseconds * NANOSECONDS;

        OSSleepTicks(OSNanosecondsToTicks(nanoseconds));
    }
}

double Timer<Console::CAFE>::Step()
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
