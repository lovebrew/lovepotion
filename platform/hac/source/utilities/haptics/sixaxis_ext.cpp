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
    auto res = hidGetSixAxisSensorHandles(this->handles.get(), this->handleCount, playerId, style);

    if (R_SUCCEEDED(res))
        this->Start();
}

SixAxis<Console::HAC>& SixAxis<Console::HAC>::operator=(SixAxis&& other)
{
    SixAxis<>::operator=(std::move(other));
    this->handles  = std::move(other.handles);
    this->playerId = other.playerId;
    this->style    = other.style;

    return *this;
}

SixAxis<Console::HAC>::~SixAxis()
{
    this->Stop();
}

bool SixAxis<Console::HAC>::Start()
{
    Result results[this->handleCount];
    for (int index = 0; index < this->handleCount; index++)
        results[index] = hidStartSixAxisSensor(this->handles[index]);

    if (this->handleCount == 1)
        return R_SUCCEEDED(results[0]);

    return R_SUCCEEDED(results[0]) && R_SUCCEEDED(results[1]);
}

void SixAxis<Console::HAC>::Stop()
{
    if (!this->handles)
        return;

    for (int index = 0; index < this->handleCount; index++)
        hidStopSixAxisSensor(this->handles[index]);
}

std::vector<Vector3> SixAxis<Console::HAC>::GetStateInfo(SixAxis<>::SixAxisType type)
{
    std::vector<Vector3> info {};

    auto fillData = [&](const int handleId) {
        HidSixAxisSensorState state {};
        hidGetSixAxisSensorStates(this->handles[handleId], &state, 1);

        if (type == SIXAXIS_ACCELEROMETER)
        {
            Vector3 temp(state.acceleration.x, state.acceleration.y, state.acceleration.z);
            info.push_back(temp);
        }
        else
        {
            Vector3 temp(state.angular_velocity.x, state.angular_velocity.y,
                         state.angular_velocity.z);
            info.push_back(temp);
        }
    };

    fillData(0);

    if (this->handleCount != 2)
        return info;

    fillData(1);

    return info;
}
