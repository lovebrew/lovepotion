#pragma once

#include <modules/timer/timer.tcc>

#include <coreinit/time.h>

namespace love
{
    template<>
    class Timer<Console::CAFE> : public Timer<Console::ALL>
    {
      public:
        Timer();

        double Step();

        void Sleep(double seconds) const;

        static double GetTime();

      private:
        static constexpr double NANOSECONDS_TO_SECONDS = 1000000000.0;

        static constexpr double NANOSECONDS  = 1000000ULL;
        static constexpr double MILLISECONDS = 1000.0;

        static OSTick reference;
    };
} // namespace love
