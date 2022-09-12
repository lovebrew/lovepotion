#pragma once

#include <modules/timer/timer.tcc>

namespace love
{
    template<>
    class Timer<Console::HAC> : public Timer<Console::ALL>
    {
      public:
        Timer();

        double Step();

        void Sleep(double seconds) const;

        static double GetTime();

      private:
        static double reference;
    };
} // namespace love
