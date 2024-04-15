#include "modules/sensor/Sensor.hpp"
#include "common/Exception.hpp"

namespace love
{
    void Sensor::setEnabled(SensorType type, bool)
    {
        std::string_view name;
        Sensor::getConstant(type, name);

        throw love::Exception("Device sensor \"{}\" does not exist.", name);
    }

    std::vector<float> Sensor::getData(SensorType type) const
    {
        std::string_view name;
        Sensor::getConstant(type, name);

        throw love::Exception("Device sensor \"{}\" does not exist.", name);
    }

    std::string_view Sensor::getSensorName(SensorType type) const
    {
        std::string_view name;
        Sensor::getConstant(type, name);

        throw love::Exception("Device sensor \"{}\" does not exist.", name);
    }
} // namespace love
