#include "common/runtime.h"
#include "objects/gamepad/handles/handles.h"

using namespace love::gamepad;

Handles::Handles(size_t id)
{
    HidControllerID identity = hidGetHandheldMode() ? CONTROLLER_HANDHELD : (HidControllerID)id;
    HidControllerType type = hidGetControllerType(identity);

    this->handleCount = HandleCounts();

    if (type & (TYPE_JOYCON_LEFT | TYPE_JOYCON_RIGHT | TYPE_PROCONTROLLER))
    {
        this->handleCount.vibration = 1;
        this->handleCount.gyro = 1;
    }
    else if (type & TYPE_JOYCON_PAIR)
    {
        this->handleCount.vibration = 2;
        this->handleCount.gyro = 2;
    }
    else if (type & TYPE_HANDHELD)
    {
        this->handleCount.vibration = 2;
        this->handleCount.gyro = 1;
    }

    memset(this->vibration.handles, 0, sizeof(this->vibration.handles));
    memset(this->vibration.values, 0, sizeof(this->vibration.values));

    hidInitializeVibrationDevices(this->vibration.handles, this->handleCount.vibration, identity, type);
    hidGetSixAxisSensorHandles(&this->gyro.handles[0], this->handleCount.gyro, identity, type);

    this->id = id;
}

Handles::~Handles()
{}

void Handles::_SetVibrationData(HidVibrationValue * vibration, float value)
{
    vibration->freq_low = 160.0f;
    vibration->freq_high = 320.0f;

    vibration->amp_low = value;
    vibration->amp_high = value;
}

bool Handles::SendVibration(const LOVE_Vibration & vibration)
{
    HidVibrationValue leftValue;
    this->_SetVibrationData(&leftValue, vibration.left);

    memcpy(&this->vibration.values[0], &leftValue, sizeof(HidVibrationValue));

    HidVibrationValue rightValue;
    this->_SetVibrationData(&rightValue, vibration.right);

    memcpy(&this->vibration.values[1], &rightValue, sizeof(HidVibrationValue));

    Result res = hidSendVibrationValues(this->vibration.handles, this->vibration.values, this->handleCount.vibration);

    return R_SUCCEEDED(res);
}

void Handles::ReadSixAxis(SixAxisSensorValues & values)
{
    hidStartSixAxisSensor(this->gyro.handles[0]);

    HidControllerID identity = hidGetHandheldMode() ? CONTROLLER_HANDHELD : (HidControllerID)this->id;

    hidSixAxisSensorValuesRead(&values, identity, this->handleCount.gyro);

    hidStopSixAxisSensor(this->gyro.handles[0]);
}
