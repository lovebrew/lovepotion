#include "common/runtime.h"
#include "objects/gamepad/gamepad.h"

using namespace love;

Gamepad::Gamepad(size_t id)
{
    this->id = id;
}

float Gamepad::GetAxis(size_t axis)
{
    float value = 0.0f;

    if (axis == 1 || axis == 2)
    {
        hidJoystickRead(&stick, CONTROLLER_P1_AUTO, JOYSTICK_LEFT);

        if (axis == 1)
             value = stick.dx;
        else if (axis == 2)
             value = stick.dy;
    }
    else if (axis == 3 || axis == 4)
    {
        hidJoystickRead(&stick, CONTROLLER_P1_AUTO, JOYSTICK_RIGHT);

        if (axis == 3)
            value = stick.dx;
        else if (axis == 4)
            value = stick.dy;
    }

    value = value / JOYSTICK_MAX;

    return std::clamp(value, -1.0f, 1.0f);
}

// TODO: expose accelerometer, etc as joystick stuff
size_t Gamepad::GetAxisCount()
{
    return 4;
}

size_t Gamepad::GetButtonCount()
{
    auto buttons = Input::buttons;

    return buttons.size();
}

float Gamepad::GetGamepadAxis(const std::string & axis)
{
    float value = 0.0f;

    if (axis.substr(0, 4) == "left")
    {
        hidJoystickRead(&stick, CONTROLLER_P1_AUTO, JOYSTICK_LEFT);

        if (axis == "leftx")
             value = stick.dx;
        else if (axis == "lefty")
             value = stick.dy;
    }
    else if (axis.substr(0, 5) == "right")
    {
        hidJoystickRead(&stick, CONTROLLER_P1_AUTO, JOYSTICK_RIGHT);

        if (axis == "rightx")
            value = stick.dx;
        else if (axis == "righty")
            value = stick.dy;
    }

    value = value / JOYSTICK_MAX;

    return std::clamp(value, -1.0f, 1.0f);
}

size_t Gamepad::GetID()
{
    return this->id;
}

std::string Gamepad::GetName()
{
    std::string ret = "Joycon";
    HidControllerType type;

    type = hidGetControllerType(CONTROLLER_P1_AUTO);

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

std::pair<float, float> Gamepad::GetVibration()
{
    return this->vibrations;
}

bool Gamepad::IsConnected()
{
    return true;
}

bool Gamepad::IsDown(size_t button)
{
    auto buttons = Input::buttons;

    for (auto it = buttons.begin(); it != buttons.end(); it++)
    {
        if (it->second & Input::GetKeyHeld<u32>())
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

    for (auto it = buttons.begin(); it != buttons.end(); it++)
    {
        if (it->second & Input::GetKeyHeld<u64>())
        {
            if (button == it->first)
                return true;
        }
    }

    return false;
}

bool Gamepad::IsVibrationSupported()
{
    return true;
}

bool Gamepad::SetVibration(float left, float right, float duration)
{
    /*
    ** TODO: implement
    */
   return false;
}
