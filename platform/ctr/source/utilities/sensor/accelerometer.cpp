#include <utilities/sensor/accelerometer.hpp>

using namespace love;

Accelerometer::Accelerometer() : data {}
{}

Accelerometer::~Accelerometer()
{
    this->SetEnabled(false);
}

std::array<float, 3> Accelerometer::GetData()
{
    hidAccelRead(&this->data);

    return { (float)this->data.x, (float)this->data.y, (float)this->data.z };
}

void Accelerometer::SetEnabled(bool enable)
{
    if (enable)
        HIDUSER_EnableAccelerometer();
    else
        HIDUSER_DisableAccelerometer();

    SensorBase::SetEnabled(enable);
}
