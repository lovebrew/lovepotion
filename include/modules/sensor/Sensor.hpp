#pragma once

#include "common/Map.hpp"
#include "common/Module.hpp"

namespace love
{
    class Sensor : public Module
    {
      public:
        enum SensorType
        {
            SENSOR_ACCELEROMETER,
            SENSOR_GYROSCOPE,
            SENSOR_MAX_ENUM
        };

        Sensor() : Module(M_SENSOR, "love.sensor")
        {}

        virtual ~Sensor()
        {}

        bool hasSensor(SensorType) const
        {
            return false;
        }

        bool isEnabled() const
        {
            return false;
        }

        void setEnabled(SensorType, bool);

        std::vector<float> getData(SensorType) const;

        std::string_view getSensorName(SensorType) const;

        // clang-format off
        STRINGMAP_DECLARE(SensorTypes, SensorType,
          { "accelerometer", SENSOR_ACCELEROMETER },
          { "gyroscope",     SENSOR_GYROSCOPE     }
        );
        // clang-format on
    };
} // namespace love
