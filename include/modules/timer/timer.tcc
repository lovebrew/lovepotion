#pragma once

#include <common/console.hpp>
#include <common/module.hpp>

#include <chrono>

namespace love
{
    template<Console::Platform T = Console::ALL>
    class Timer : public Module
    {
      public:
        Timer() :
            currentTime(0),
            previousFpsUpdate(0),
            fps(0),
            averageDelta(0),
            fpsUpdateFrequency(1),
            frames(0),
            delta(0)
        {}

        virtual ~Timer()
        {}

        const char* GetName() const override
        {
            return "love.timer";
        }

        ModuleType GetModuleType() const override
        {
            return M_TIMER;
        }

        double GetDelta() const
        {
            return this->delta;
        }

        double GetAverageDelta() const
        {
            return this->averageDelta;
        }

        int GetFPS() const
        {
            return this->fps;
        }

      protected:
        double currentTime;
        double previousFpsUpdate;

        double previousTime;

        int fps;
        double averageDelta;
        double fpsUpdateFrequency;

        int frames;
        double delta;
    };
} // namespace love
