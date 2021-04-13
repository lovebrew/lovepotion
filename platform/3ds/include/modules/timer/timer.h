#pragma once

#include "modules/timer/timerc.h"
#include <3ds.h>

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