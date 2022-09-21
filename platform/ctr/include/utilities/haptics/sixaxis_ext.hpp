#pragma once

#include <common/vector.hpp>

#include <utilities/haptics/sixaxis.tcc>

#include <3ds.h>

namespace love
{
    template<>
    class SixAxis<Console::CTR> : public SixAxis<Console::ALL>
    {
      public:
        SixAxis();

        Vector3 GetInfo(SixAxisType type);

      private:
        accelVector accelerometer;
        angularRate gyroscope;
    };
} // namespace love
