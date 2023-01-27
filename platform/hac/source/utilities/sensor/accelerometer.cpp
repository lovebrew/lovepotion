#include <utilities/sensor/accelerometer.hpp>

using namespace love;

Accelerometer::Accelerometer(HidNpadIdType playerId, HidNpadStyleTag style) :
    playerId(playerId),
    style(style)
{
    this->handleCount = 1;

    if (style == HidNpadStyleTag_NpadJoyDual)
        this->handleCount = 2;

    this->handles = std::make_unique<HidSixAxisSensorHandle[]>(this->handleCount);
    hidGetSixAxisSensorHandles(this->handles.get(), this->handleCount, playerId, style);
}

Accelerometer& Accelerometer::operator=(Accelerometer&& other)
{
    Accelerometer::operator=(std::move(other));
    this->handles  = std::move(other.handles);
    this->playerId = other.playerId;
    this->style    = other.style;

    return *this;
}

Accelerometer::~Accelerometer()
{
    this->SetEnabled(false);
}

void Accelerometer::SetEnabled(bool enabled)
{
    if (enabled)
    {
        for (int index = 0; index < this->handleCount; index++)
            hidStartSixAxisSensor(this->handles[index]);
    }
    else
    {
        if (!this->handles)
            return;

        for (int index = 0; index < this->handleCount; index++)
            hidStopSixAxisSensor(this->handles[index]);
    }

    SensorBase::SetEnabled(enabled);
}

std::vector<float> Accelerometer::GetData()
{
    std::vector<float> results {};

    // clang-format off
    for (int index = 0; index < this->handleCount; index++)
    {
        HidSixAxisSensorState state {};
        hidGetSixAxisSensorStates(this->handles[index], &state, 1);

        results.insert(results.end(), { state.acceleration.x, state.acceleration.y, state.acceleration.z });
    }
    // clang-format on

    return results;
}
