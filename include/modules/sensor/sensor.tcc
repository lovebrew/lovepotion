#pragma once

#include <common/console.hpp>
#include <common/module.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <utilities/sensor/sensorbase.hpp>

#include <map>
#include <vector>

namespace love
{
    template<Console::Platform = Console::ALL>
    class Sensor : public Module
    {
      public:
        enum SensorType
        {
            SENSOR_ACCELEROMETER,
            SENSOR_GYROSCOPE
        };

        Sensor() : sensors()
        {}

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

        bool HasSensor(SensorType type)
        {}

        bool IsEnabled(SensorType type)
        {
            return this->sensors[type];
        }

        const char* GetSensorName(SensorType type)
        {
            auto name = Sensor<>::sensorTypes.ReverseFind(type);

            if (!this->sensors[type])
                throw love::Exception("\"%s\" sensor is not enabled.", *name);

            return *name;
        }

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
