#pragma once

#include <3ds/os.h>

#include <modules/timer/timer.tcc>

namespace love
{
    template<>
    class Timer<Console::CTR> : public Timer<Console::ALL>
    {
      public:
        Timer();

        double Step();

        void Sleep(double seconds) const;

        static double GetTime();

      private:
        static TickCounter counter;
    };
} // namespace love
