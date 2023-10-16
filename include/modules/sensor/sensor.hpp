#pragma once

#include <common/console.hpp>
#include <common/module.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <utilities/sensor/sensorbase.hpp>

#include <map>
#include <vector>

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

        Sensor();

        virtual ~Sensor()
        {}

        const char* GetName() const override
        {
            return "love.sensor";
        }

        ModuleType GetModuleType() const override
        {
            return M_SENSOR;
        }

        bool HasSensor(SensorType type);

        bool IsEnabled(SensorType type);

        const char* GetSensorName(SensorType type);

        void SetEnabled(SensorType type, bool enable);

        std::array<float, 3> GetData(SensorType type);

        // clang-format off
        static constexpr BidirectionalMap sensorTypes = {
            "accelerometer", SENSOR_ACCELEROMETER,
            "gyroscope",     SENSOR_GYROSCOPE
        };
        // clang-format on

      protected:
        std::map<SensorType, SensorBase*> sensors;
    };
} // namespace love
