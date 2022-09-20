#pragma once

#include <common/console.hpp>
#include <common/vector.hpp>

#include <memory>

namespace love
{
    template<Console::Platform T = Console::ALL>
    class SixAxis
    {
      public:
        enum SixAxisType
        {
            SIXAXIS_GYROSCOPE,
            SIXAXIS_ACCELEROMETER,
            SIXAXIS_MAX_ENUM
        };

      protected:
        int handleCount;
    };
} // namespace love
