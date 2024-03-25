#include "modules/timer/Timer.hpp"

namespace love
{
    TickCounter Timer::counter {};

    Timer::Timer()
    {
        osTickCounterStart(&counter);
        this->prevFpsUpdate = this->currTime = Timer::getTime();
    }

    double Timer::getTime()
    {
        counter.elapsed = svcGetSystemTick() - counter.reference;
        return osTickCounterRead(&counter) / 1000.0;
    }

    void Timer::sleep(double seconds) const
    {
        if (seconds >= 0)
        {
            auto time = std::chrono::duration<double>(seconds);
            svcSleepThread((int64_t)std::chrono::duration<double, std::nano>(time).count());
        }
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
