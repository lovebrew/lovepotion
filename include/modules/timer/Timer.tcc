#pragma once

#include "common/Module.hpp"

#include <chrono>
using namespace std::chrono_literals;

namespace love
{
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

        static constexpr double SECONDS_TO_NS = 1.0E9;
    };
} // namespace love
