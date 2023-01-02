#include <utilities/haptics/vibration_ext.hpp>

using namespace love;

Vibration<Console::HAC>::Vibration(HidNpadIdType playerId, HidNpadStyleTag style) :
    playerId(playerId),
    style(style)
{
    this->handleCount = 1;

    if (style == HidNpadStyleTag_NpadJoyDual || style == HidNpadStyleTag_NpadHandheld)
        this->handleCount = 2;

    this->handles = std::make_unique<HidVibrationDeviceHandle[]>(this->handleCount);
    hidInitializeVibrationDevices(this->handles.get(), this->handleCount, playerId, style);
}

Vibration<Console::HAC>::~Vibration()
{
    if (!this->handles)
        return;

    this->Stop();
}

Vibration<Console::HAC>& Vibration<Console::HAC>::operator=(Vibration&& other)
{
    Vibration<>::operator=(std::move(other));
    this->handles  = std::move(other.handles);
    this->playerId = other.playerId;
    this->style    = other.style;

    return *this;
}

bool Vibration<Console::HAC>::Stop()
{
    return this->SendValues(0, 0);
}

bool Vibration<Console::HAC>::SendValues(float left, float right)
{
    if (!this->handles)
        return false;

    HidVibrationValue values[this->handleCount] {};

    for (auto& value : values)
    {
        value.freq_low  = 160.0f;
        value.freq_high = 320.0f;

        value.amp_low  = left;
        value.amp_high = right;
    }

    Result result = hidSendVibrationValues(this->handles.get(), values, this->handleCount);

    if (R_SUCCEEDED(result))
    {
        this->vibrationInfo.left  = left;
        this->vibrationInfo.right = right;
    }
    else
    {
        this->vibrationInfo.left = this->vibrationInfo.right = 0.0f;
        this->vibrationInfo.endTime                          = -1.0f;
    }

    return R_SUCCEEDED(result);
}
