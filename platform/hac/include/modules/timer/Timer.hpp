#pragma once

#include "modules/timer/Timer.tcc"

namespace love
{
    class Timer : public TimerBase
    {
      public:
        Timer();

        double step();

        void sleep(double seconds) const;

        static double getTime();

      private:
        static double reference;
    };
} // namespace love
