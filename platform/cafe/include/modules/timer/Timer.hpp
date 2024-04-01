#pragma once

#include "modules/timer/Timer.tcc"

#include <coreinit/time.h>

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
        static OSTick reference;
    };
} // namespace love
