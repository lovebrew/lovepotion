#include <switch.h>

#include <chrono>
using namespace std::chrono_literals;

#include <modules/timer_ext.hpp>

using namespace love;

double Timer<Console::HAC>::reference = 0;

Timer<Console::HAC>::Timer()
{
    Timer::reference        = armGetSystemTick();
    this->previousFpsUpdate = this->currentTime = Timer::GetTime();
}

double Timer<Console::HAC>::GetTime()
{
    uint64_t nanoseconds = armTicksToNs(armGetSystemTick() - Timer::reference);
    return nanoseconds / Timer::NANOSECONDS_TO_SECONDS;
}

void Timer<Console::HAC>::Sleep(double seconds) const
{
    if (seconds >= 0)
    {
        auto time = std::chrono::duration<double>(seconds);
        svcSleepThread(std::chrono::duration<double, std::nano>(time).count());
    }
}

double Timer<Console::HAC>::Step()
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
