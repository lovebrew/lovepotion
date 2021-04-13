#pragma once

#include "modules/timer/timerc.h"

namespace love
{
    class Timer : public common::Timer
    {
      public:
        Timer();

        virtual ~Timer()
        {}
    };
} // namespace love