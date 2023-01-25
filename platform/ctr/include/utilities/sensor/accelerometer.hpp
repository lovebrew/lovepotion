#pragma once

#include <common/vector.hpp>
#include <utilities/sensor/sensorbase.hpp>

#include <3ds.h>

namespace love
{
    class Accelerometer : public SensorBase
    {
      public:
        Accelerometer();

        ~Accelerometer();

        std::array<float, 3> GetData() override;

        void SetEnabled(bool enable);

        bool IsEnabled() const;

      private:
        accelVector data;
    };
} // namespace love
