#pragma once

#include "modules/timer/Timer.tcc"

#include <3ds.h>

namespace love
{
    class Timer : public TimerBase<Timer>
    {
      public:
        Timer();

        double step();

        void sleep(double seconds) const;

        static double getTime();

      private:
        static TickCounter counter;
    };
} // namespace love
