/*
** modules/timer.h
** @brief : Used for time keeping
*/

#pragma once

#include "common/module.h"

#if defined (_3DS)
    #include <3ds.h>
#elif defined (__SWITCH__)
    #include <switch.h>
#endif

namespace love
{
    class Timer : public Module
    {
        public:
            Timer();

            ModuleType GetModuleType() const { return M_TIMER; }

            const char * GetName() const override { return "love.timer"; }

            //Löve2D Functions

            double GetAverageDelta();

            double GetDelta();

            int GetFPS();

            static double GetTime();

            void Sleep(float seconds);

            double Step();

            //End Löve2D Functions

        private:
            #if defined (_3DS)
                static TickCounter counter;
            #endif

            static u64 reference;

            double currentTime;
            double lastTime;
            double prevFPSUpdate;

            int fps;
            double averageDelta;

            double fpsUpdateFrequency;
            int frames;

            double dt;
    };
}
