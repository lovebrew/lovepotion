#pragma once

#include <modules/sensor/sensor.tcc>

#include <utilities/sensor/accelerometer.hpp>
#include <utilities/sensor/gyroscope.hpp>

namespace love
{
    template<>
    class Sensor<Console::CTR> : public Sensor<Console::ALL>
    {
      public:
        Sensor();

        void SetEnabled(SensorType type, bool enable);

        std::array<float, 3> GetData(SensorType type);
    };
} // namespace love
