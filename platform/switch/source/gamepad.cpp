#include "common/runtime.h"
#include "objects/gamepad/gamepad.h"

using namespace love;

Gamepad::Gamepad(size_t id) : duration(0)
{
    this->id = id;
}

float Gamepad::GetAxis(size_t axis)
{
    float value = 0.0f;

    HidControllerID hidID = CONTROLLER_P1_AUTO;
    if (this->id != 0)
        hidID = static_cast<HidControllerID>(this->id);

    if (axis < 5)
    {
        if (axis == 1 || axis == 2)
        {
            hidJoystickRead(&stick, hidID, JOYSTICK_LEFT);

            if (axis == 1)
                value = stick.dx;
            else if (axis == 2)
                value = stick.dy;
        }
        else if (axis == 3 || axis == 4)
        {
            hidJoystickRead(&stick, hidID, JOYSTICK_RIGHT);

            if (axis == 3)
                value = stick.dx;
            else if (axis == 4)
                value = stick.dy;
        }

        value = value / (float)JOYSTICK_MAX;

        return value;
    }
    else if (axis == 5)
    {
        if (Input::GetKeyHeld<u64>() & KEY_ZL)
            return 1.0f;
        else if (Input::GetKeyUp<u64>() & KEY_ZL)
            return 0.0f;
    }
    else if (axis == 6)
    {
        if (Input::GetKeyHeld<u64>() & KEY_ZR)
            return 1.0f;
        else if (Input::GetKeyUp<u64>() & KEY_ZR)
            return 0.0f;
    }
    else
    {
        if (!g_accelJoystick)
            return 0.0f;

        if (this->handles == nullptr)
            this->handles = std::make_unique<gamepad::Handles>(this->id);

        SixAxisSensorValues values;
        this->handles->ReadSixAxis(values);

        if (axis >= 7 && axis < 10)
        {
            if (axis == 7)
                return values.gyroscope.x;
            else if (axis == 8)
                return values.gyroscope.y;

            return values.gyroscope.z;
        }
        else if (axis >= 10 && axis < 13)
        {
            if (axis == 10)
                return values.accelerometer.x;
            else if (axis == 11)
                return values.accelerometer.y;

            return values.accelerometer.z;
        }
    }

    return 0.0f;
}

size_t Gamepad::GetAxisCount()
{
    return 12;
}

size_t Gamepad::GetButtonCount()
{
    auto buttons = Input::buttons;

    return buttons.size();
}

float Gamepad::GetGamepadAxis(const std::string & axis)
{
    float value = 0.0f;

    HidControllerID hidID = CONTROLLER_P1_AUTO;
    if (this->id != 0)
        hidID = static_cast<HidControllerID>(this->id);

    if (axis.substr(0, 4) == "left")
    {
        hidJoystickRead(&stick, hidID, JOYSTICK_LEFT);

        if (axis == "leftx")
             value = stick.dx;
        else if (axis == "lefty")
             value = stick.dy;
    }
    else if (axis.substr(0, 5) == "right")
    {
        hidJoystickRead(&stick, hidID, JOYSTICK_RIGHT);

        if (axis == "rightx")
            value = stick.dx;
        else if (axis == "righty")
            value = stick.dy;
    }

    return value / (float)JOYSTICK_MAX;
}

size_t Gamepad::GetID()
{
    return this->id;
}

std::string Gamepad::GetName()
{
    std::string ret = "Joycon";
    HidControllerType type;

    HidControllerID hidID = CONTROLLER_P1_AUTO;
    if (this->id != 0)
        hidID = static_cast<HidControllerID>(this->id);

    type = hidGetControllerType(hidID);

    switch (type)
    {
        case TYPE_PROCONTROLLER:
            ret = "Pro Controller";
            break;
        case TYPE_HANDHELD:
            ret = "Handheld";
            break;
        case TYPE_JOYCON_LEFT:
            ret = "Joycon Left";
            break;
        case TYPE_JOYCON_RIGHT:
            ret = "Joycon Right";
            break;
        default:
            break;
    }

    return ret;
}

LOVE_Vibration Gamepad::GetVibration()
{
    return this->vibration;
}

bool Gamepad::IsConnected()
{
    HidControllerID hidID = CONTROLLER_P1_AUTO;
    if (this->id != 0)
        hidID = static_cast<HidControllerID>(this->id);

    return hidIsControllerConnected(hidID);
}

bool Gamepad::IsDown(size_t button)
{
    auto buttons = Input::buttons;

    HidControllerID hidID = CONTROLLER_P1_AUTO;
    if (this->id != 0)
        hidID = static_cast<HidControllerID>(this->id);

    u64 keyHeld = hidKeysHeld(hidID);

    for (auto it = buttons.begin(); it != buttons.end(); it++)
    {
        if (it->second & keyHeld)
        {
            size_t index = std::distance(it, buttons.begin()) - 1;

            if (button == index)
                return true;
        }
    }

    return false;
}

bool Gamepad::IsGamepad()
{
    return true;
}

bool Gamepad::IsGamepadDown(const std::string & button)
{
    auto buttons = Input::buttons;

    HidControllerID hidID = CONTROLLER_P1_AUTO;
    if (this->id != 0)
        hidID = static_cast<HidControllerID>(this->id);

    u64 keyHeld = hidKeysHeld(hidID);

    for (auto it = buttons.begin(); it != buttons.end(); it++)
    {
        if (it->second & keyHeld)
        {
            if (button == it->first)
                return true;
        }
    }

    return false;
}

float Gamepad::GetVibrationDuration() const
{
    return this->duration;
}

bool Gamepad::IsVibrationSupported()
{
    return true;
}

void Gamepad::SetVibrationValues(const LOVE_Vibration & vibration)
{
    this->vibration = vibration;
}

bool Gamepad::SyncVibration(float duration)
{
    if (this->handles == nullptr)
        this->handles = std::make_unique<gamepad::Handles>(this->id);

    this->duration = duration;

    if (duration == 0)
    {
        this->vibration.left = 0.0f;
        this->vibration.right = 0.0f;
    }

    return this->handles->SendVibration(this->vibration);
}
