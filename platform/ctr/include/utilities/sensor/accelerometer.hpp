#pragma once

#include <utilities/sensor/sensorbase.hpp>

#include <3ds.h>

namespace love
{
    class Accelerometer : public SensorBase
    {
      public:
        Accelerometer();

        ~Accelerometer();

        std::vector<float> GetData() override;

        void SetEnabled(bool enable);

        bool IsEnabled() const;

      private:
        accelVector data;
    };
} // namespace love
