#include "modules/timer/Timer.hpp"

#include <switch.h>

namespace love
{
    double Timer::reference = 0;

    Timer::Timer()
    {
        Timer::reference    = armGetSystemTick();
        this->prevFpsUpdate = this->currTime = Timer::getTime();
    }

    double Timer::getTime()
    {
        const auto ns = armTicksToNs(armGetSystemTick() - Timer::reference);
        return ns / Timer::SECONDS_TO_NS;
    }

    void Timer::sleep(double seconds) const
    {
        const auto time = std::chrono::duration<double>(seconds);
        svcSleepThread(std::chrono::duration<double, std::nano>(time).count());
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
