#include <algorithm>

#include <utilities/haptics/sixaxis.hpp>

using namespace love;

SixAxis::SixAxis(HidNpadIdType playerId, HidNpadStyleTag style)
{
    this->handleCount = 1;

    if (style == HidNpadStyleTag_NpadJoyDual)
        this->handleCount = 2;

    this->handles = std::make_unique<HidSixAxisSensorHandle[]>(this->handleCount);
    hidGetSixAxisSensorHandles(this->handles.get(), this->handleCount, playerId, style);

    for (int index = 0; index < this->handleCount; index++)
        hidStartSixAxisSensor(this->handles[index]);
}

SixAxis::SixAxisInfo SixAxis::GetStateInfo()
{
    HidSixAxisSensorState state {};
    SixAxisInfo info {};

    hidGetSixAxisSensorStates(this->handles[0], &state, 1);

    std::copy_n(&state.acceleration, 1, &info.acceleration[0]);
    std::copy_n(&state.angular_velocity, 1, &info.velocity[0]);

    if (this->handleCount != 2)
        return info;

    hidGetSixAxisSensorStates(this->handles[1], &state, 1);

    std::copy_n(&state.acceleration, 1, &info.acceleration[1]);
    std::copy_n(&state.angular_velocity, 1, &info.velocity[1]);

    return info;
}

SixAxis::~SixAxis()
{
    for (int index = 0; index < this->handleCount; index++)
        hidStopSixAxisSensor(this->handles[index]);
}
