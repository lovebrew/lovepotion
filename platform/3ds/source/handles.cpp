#include "common/runtime.h"
#include "objects/gamepad/handles/handles.h"

using namespace love::gamepad;

Handles::Handles(size_t)
{}

Handles::~Handles()
{}

bool Handles::SendVibration(const LOVE_Vibration & vibration)
{
    /* Vibration isn't possible on 3DS */

    return false;
}

void Handles::ReadSixAxis(SixAxisSensorValues & values)
{
    hidAccelRead(&values.accelerometer);
    hidGyroRead(&values.gyroscope);
}
