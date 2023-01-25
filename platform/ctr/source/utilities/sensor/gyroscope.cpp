#include <utilities/sensor/gyroscope.hpp>

using namespace love;

Gyroscope::Gyroscope() : data {}
{}

Gyroscope::~Gyroscope()
{
    this->SetEnabled(false);
}

std::array<float, 3> Gyroscope::GetData()
{
    hidGyroRead(&this->data);

    return { this->data.x, this->data.y, this->data.z };
}

void Gyroscope::SetEnabled(bool enabled)
{
    if (enabled)
        HIDUSER_EnableGyroscope();
    else
        HIDUSER_DisableGyroscope();

    SensorBase::SetEnabled(enabled);
}
