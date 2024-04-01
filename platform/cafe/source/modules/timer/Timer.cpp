#include "modules/timer/Timer.hpp"

#include <coreinit/thread.h>

namespace love
{
    OSTick Timer::reference = 0;

    Timer::Timer()
    {
        Timer::reference    = OSGetSystemTick();
        this->prevFpsUpdate = this->currTime = Timer::getTime();
    }

    double Timer::getTime()
    {
        const auto ns = OSTicksToNanoseconds(OSGetSystemTick() - Timer::reference);
        return ns / Timer::SECONDS_TO_NS;
    }

    void Timer::sleep(double seconds) const
    {
        const auto time        = std::chrono::duration<double>(seconds);
        const auto nanoseconds = std::chrono::duration<double, std::nano>(time).count();

        OSSleepTicks(OSNanosecondsToTicks(nanoseconds));
    }

    double Timer::step()
    {
        this->frames++;

        this->prevTime = this->currTime;
        this->currTime = Timer::getTime();

        this->dt = this->currTime - this->prevTime;

        double timeSinceLast = (this->currTime - this->prevFpsUpdate);

        if (timeSinceLast > this->fpsUpdateFrequency)
        {
            this->fps           = int((this->frames / timeSinceLast) + 0.5);
            this->averageDelta  = timeSinceLast / frames;
            this->prevFpsUpdate = this->currTime;
            this->frames        = 0;
        }

        return this->dt;
    }
} // namespace love
