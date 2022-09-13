#include <modules/timer_ext.hpp>

#include <coreinit/thread.h>
#include <coreinit/time.h>

using namespace love;

double Timer<Console::CAFE>::reference = 0;

Timer<Console::CAFE>::Timer()
{
    Timer::reference        = OSGetSystemTick();
    this->previousFpsUpdate = this->currentTime = Timer::GetTime();
}

double Timer<Console::CAFE>::GetTime()
{
    std::chrono::nanoseconds nanosec(OSTicksToNanoseconds(OSGetSystemTick() - Timer::reference));
    return std::chrono::duration_cast<std::chrono::seconds>(nanosec).count();
}

void Timer<Console::CAFE>::Sleep(double seconds) const
{
    if (seconds >= 0)
    {
        auto time = std::chrono::duration<double>(seconds);
        OSSleepTicks(OSNanosecondsToTicks(std::chrono::duration<double, std::nano>(time).count()));
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
