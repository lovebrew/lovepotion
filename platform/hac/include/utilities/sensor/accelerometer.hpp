#pragma once

#include <utilities/sensor/sensorbase.hpp>

#include <switch.h>

#include <memory>

namespace love
{
    class Accelerometer : public SensorBase
    {
      public:
        Accelerometer()
        {}

        Accelerometer(HidNpadIdType playerId, HidNpadStyleTag style);

        ~Accelerometer();

        Accelerometer& operator=(Accelerometer&& other);

        void SetEnabled(bool enabled);

        std::vector<float> GetData() override;

      private:
        std::unique_ptr<HidSixAxisSensorHandle[]> handles;

        HidNpadIdType playerId;
        HidNpadStyleTag style;

        int handleCount;
    };
} // namespace love
