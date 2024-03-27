#pragma once

#include "common/Module.hpp"

#include <chrono>

namespace love
{
    template<class T>
    class TimerBase : public Module
    {
      public:
        TimerBase() :
            Module(M_TIMER, "love.timer"),
            currTime(0),
            prevFpsUpdate(0),
            fps(0),
            averageDelta(0),
            fpsUpdateFrequency(1),
            frames(0),
            dt(0)
        {}

        virtual ~TimerBase()
        {}

        double getDelta() const
        {
            return this->dt;
        }

        double getAverageDelta() const
        {
            return this->averageDelta;
        }

        double getFPS() const
        {
            return this->fps;
        }

      protected:
        double currTime;
        double prevTime;
        double prevFpsUpdate;

        int fps;
        double averageDelta;

        double fpsUpdateFrequency;

        int frames;

        double dt;
    };
} // namespace love
