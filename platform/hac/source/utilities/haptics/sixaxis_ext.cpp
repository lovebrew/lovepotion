#include <algorithm>

#include <utilities/haptics/sixaxis_ext.hpp>

using namespace love;

SixAxis<Console::HAC>::SixAxis(HidNpadIdType playerId, HidNpadStyleTag style) :
    playerId(playerId),
    style(style)
{
    this->handleCount = 1;

    if (style == HidNpadStyleTag_NpadJoyDual)
        this->handleCount = 2;

    this->handles = std::make_unique<HidSixAxisSensorHandle[]>(this->handleCount);
    hidGetSixAxisSensorHandles(this->handles.get(), this->handleCount, playerId, style);

    this->Start();
}

SixAxis<Console::HAC>::SixAxis(SixAxis&& other) :
    SixAxis<>(other),
    handles(std::move(other.handles)),
    playerId(other.playerId),
    style(other.style)
{}

SixAxis<Console::HAC>::~SixAxis()
{
    this->Stop();
}

bool SixAxis<Console::HAC>::Start()
{
    for (int index = 0; index < this->handleCount; index++)
        hidStartSixAxisSensor(this->handles[index]);
}

void SixAxis<Console::HAC>::Stop()
{
    if (!this->handles)
        return;

    for (int index = 0; index < this->handleCount; index++)
        hidStopSixAxisSensor(this->handles[index]);
}

std::pair<Vector3, Vector3> SixAxis<Console::HAC>::GetStateInfo(SixAxis<>::SixAxisType type)
{
    std::pair<Vector3, Vector3> info {};

    auto fillData = [&](auto& dataInfo, const int handleId) {
        HidSixAxisSensorState state {};
        hidGetSixAxisSensorStates(this->handles[handleId], &state, 1);

        if (type == SIXAXIS_ACCELEROMETER)
        {
            dataInfo.x = state.acceleration.x;
            dataInfo.y = state.acceleration.y;
            dataInfo.z = state.acceleration.z;
        }
        else
        {
            dataInfo.x = state.angular_velocity.x;
            dataInfo.y = state.angular_velocity.y;
            dataInfo.z = state.angular_velocity.z;
        }
    };

    fillData(info.first, 0);

    if (this->handleCount != 2)
        return info;

    fillData(info.second, 1);

    return info;
}
