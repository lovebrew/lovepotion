#include <modules/sensor/sensor.hpp>

using namespace love;

Sensor::Sensor() : sensors()
{}

bool Sensor::HasSensor(SensorType type)
{
    return false;
}

bool Sensor::IsEnabled(SensorType type)
{
    return this->sensors[type];
}

void Sensor::SetEnabled(SensorType type, bool enable)
{
    if (this->sensors[type] && !enable)
        this->sensors[type] = nullptr;
    else if (this->sensors[type] == nullptr && enable)
    {
        auto name = Sensor::sensorTypes.ReverseFind(type);
        throw love::Exception("Could not open \"%s\" sensor", *name);
    }
}

std::array<float, 3> Sensor::GetData(SensorType type)
{
    if (this->sensors[type] == nullptr)
    {
        auto name = Sensor::sensorTypes.ReverseFind(type);
        throw love::Exception("\"%s\" sensor is not enabled", *name);
    }

    return { 0.0f, 0.0f, 0.0f };
}

const char* Sensor::GetSensorName(SensorType type)
{
    auto name = Sensor::sensorTypes.ReverseFind(type);

    if (this->sensors[type] == nullptr)
        throw love::Exception("\"%s\" sensor is not enabled", *name);

    return *name;
}
