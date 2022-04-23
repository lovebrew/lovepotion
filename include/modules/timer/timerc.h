/*
** modules/timer.h
** @brief : Used for time keeping
*/

#pragma once

#include "common/module.h"

namespace love::common
{
    class Timer : public Module
    {
      public:
        static constexpr auto SLEEP_DURATION = 1000000ULL;

        Timer();

        ModuleType GetModuleType() const
        {
            return M_TIMER;
        }

        const char* GetName() const override
        {
            return "love.timer";
        }

        // Löve2D Functions

        static double GetTime();

        double GetAverageDelta();

        double GetDelta();

        int GetFPS();

        void Sleep(float seconds);

        double Step();

        // End Löve2D Functions

      protected:
        double currentTime;
        double lastTime;
        double prevFPSUpdate;

        int fps;
        double averageDelta;

        double fpsUpdateFrequency;
        int frames;

        double dt;

        static uint64_t reference;
    };
} // namespace love::common
