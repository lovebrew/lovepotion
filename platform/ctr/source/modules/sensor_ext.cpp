#include <modules/sensor_ext.hpp>

using namespace love;

Sensor<Console::CTR>::Sensor()
{}

void Sensor<Console::CTR>::SetEnabled(SensorType type, bool enable)
{
    if (this->sensors[type] && !enable)
        this->sensors[type] = nullptr;
    else if (this->sensors[type] == nullptr && enable)
    {
        SensorBase* sensor = nullptr;

        if (type == SENSOR_ACCELEROMETER)
            sensor = new Accelerometer();
        else
            sensor = new Gyroscope();

        sensor->SetEnabled(enable);
        this->sensors[type] = sensor;
    }
}

std::array<float, 3> Sensor<Console::CTR>::GetData(SensorType type)
{
    if (!this->sensors[type])
    {
        std::optional<const char*> name;
        Sensor<>::sensorTypes.ReverseFind(type);

        throw love::Exception("\"%s\" sensor is not enabled.", name);
    }

    return this->sensors[type]->GetData();
}
