#include <utilities/haptics/sixaxis_ext.hpp>

using namespace love;

SixAxis<Console::CTR>::SixAxis() : accelerometer {}, gyroscope {}
{}

Vector3 SixAxis<Console::CTR>::GetInfo(SixAxisType type)
{
    Vector3 info {};

    if (type == SixAxis<>::SIXAXIS_ACCELEROMETER)
    {
        hidAccelRead(&this->accelerometer);

        info.x = this->accelerometer.x;
        info.y = this->accelerometer.y;
        info.z = this->accelerometer.z;

        return info;
    }

    hidGyroRead(&this->gyroscope);

    info.x = this->gyroscope.x;
    info.y = this->gyroscope.y;
    info.z = this->gyroscope.z;

    return info;
}
