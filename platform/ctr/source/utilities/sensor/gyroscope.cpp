#include <utilities/sensor/gyroscope.hpp>

using namespace love;

Gyroscope::Gyroscope() : data {}
{}

Gyroscope::~Gyroscope()
{
    this->SetEnabled(false);
}

std::vector<float> Gyroscope::GetData()
{
    hidGyroRead(&this->data);

    return { (float)this->data.x, (float)this->data.y, (float)this->data.z };
}

void Gyroscope::SetEnabled(bool enabled)
{
    if (enabled)
        HIDUSER_EnableGyroscope();
    else
        HIDUSER_DisableGyroscope();

    SensorBase::SetEnabled(enabled);
}
