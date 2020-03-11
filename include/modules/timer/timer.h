/*
** modules/timer.h
** @brief : Used for time keeping
*/

#pragma once

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

            double GetTime();

            void Sleep(float seconds);

            double Step();

            //End Löve2D Functions

        private:
            double currentTime;
            double lastTime;
            double prevFPSUpdate;

            int fps;
            double averageDelta;

            double fpsUpdateFrequency;
            int frames;

            double dt;
            u64 reference;

            TickCounter counter;
    };
}
