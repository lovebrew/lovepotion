#pragma once

#include <utilities/sensor/sensorbase.hpp>

#include <3ds.h>

namespace love
{
    class Gyroscope : public SensorBase
    {
      public:
        Gyroscope();

        ~Gyroscope();

        std::vector<float> GetData() override;

        void SetEnabled(bool enabled);

      private:
        angularRate data;
    };
} // namespace love
