#pragma once

#include <utilities/sensor/sensorbase.hpp>

#include <switch.h>

#include <memory>
#include <vector>

namespace love
{
    class Gyroscope : public SensorBase
    {
      public:
        Gyroscope()
        {}

        Gyroscope(HidNpadIdType playerId, HidNpadStyleTag style);

        ~Gyroscope();

        Gyroscope& operator=(Gyroscope&& other);

        void SetEnabled(bool enabled);

        std::vector<float> GetData() override;

        int GetHandleCount()
        {
            return this->handleCount;
        }

      private:
        std::unique_ptr<HidSixAxisSensorHandle[]> handles;

        HidNpadIdType playerId;
        HidNpadStyleTag style;

        int handleCount;
    };
} // namespace love
