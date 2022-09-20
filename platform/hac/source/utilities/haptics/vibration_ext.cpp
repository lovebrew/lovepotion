#include <utilities/haptics/vibration_ext.hpp>

using namespace love;

Vibration<Console::HAC>::Vibration(HidNpadIdType playerId, HidNpadStyleTag style) :
    playerId(playerId),
    style(style)
{
    this->handleCount = 1;

    if (style == HidNpadStyleTag_NpadJoyDual || style == HidNpadStyleTag_NpadHandheld)
        this->handleCount = 2;

    this->handles = std::make_unique<HidVibrationDeviceHandle[]>(2);

    hidInitializeVibrationDevices(this->handles.get(), this->handleCount, playerId, style);
}

Vibration<Console::HAC>::Vibration(Vibration&& other) :
    Vibration<>(other),
    handles(std::move(other.handles)),
    playerId(other.playerId),
    style(other.style)
{}

bool Vibration<Console::HAC>::SendValues(float left, float right)
{
    HidVibrationValue values[this->handleCount] {};

    for (auto& value : values)
    {
        value.freq_low  = 160.0f;
        value.freq_high = 320.0f;

        value.amp_low  = left;
        value.amp_high = right;
    }

    Result success = hidSendVibrationValues(this->handles.get(), values, this->handleCount);

    if (success)
    {
        this->vibrationInfo.left  = left;
        this->vibrationInfo.right = right;
    }
    else
    {
        this->vibrationInfo.left = this->vibrationInfo.right = 0.0f;
        this->vibrationInfo.endTime                          = -1.0f;
    }

    return R_SUCCEEDED(success);
}

Vibration<Console::HAC>::~Vibration()
{}
