#include <utilities/haptics/sixaxis_ext.hpp>

using namespace love;

SixAxis<Console::CAFE>::SixAxis(bool isGamepad)
{
    this->isGamepad = isGamepad;
}

void SixAxis<Console::CAFE>::Update(VPADVec3D vector, SixAxisType type)
{
    if (type == SixAxis::SIXAXIS_ACCELEROMETER)
        this->accelerometer = vector;
    else
        this->gyroscope = vector;
}

void SixAxis<Console::CAFE>::Update(KPADVec3D vector, SixAxisType type)
{
    if (type == SixAxis::SIXAXIS_ACCELEROMETER)
        this->kpadAccelerometer = vector;
}

Vector3 SixAxis<Console::CAFE>::GetInfo(SixAxisType type)
{
    if (this->isGamepad)
    {
        if (type == SixAxis::SIXAXIS_ACCELEROMETER)
            return Vector3(this->accelerometer.x, this->accelerometer.y, this->accelerometer.z);

        return Vector3(this->gyroscope.x, this->gyroscope.y, this->gyroscope.z);
    }

    return Vector3(this->kpadAccelerometer.x, this->kpadAccelerometer.y, this->kpadAccelerometer.z);
}
