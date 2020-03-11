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
        hidCircleRead(&stick);

        if (axis == 1)
            value = stick.dx;
        else if (axis == 2)
            value = stick.dy;
    }
    else if (axis == 3 || axis == 4)
    {
        irrstCstickRead(&stick);

        if (axis == 3)
            value = stick.dx;
        else if (axis == 3)
            value = stick.dy;
    }

    value = value / JOYSTICK_MAX;

    return std::clamp(value, -1.0f, 1.0f);
}

// TODO: expose accelerometer, etc as joystick stuff
size_t Gamepad::GetAxisCount()
{
    bool isN3DS = false;
    APT_CheckNew3DS(&isN3DS);

    if (!isN3DS)
        return 2;

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
        hidCircleRead(&stick);

        if (axis == "leftx")
            value = stick.dx;
        else if (axis == "lefty")
            value = stick.dy;
    }
    else if (axis.substr(0, 5) == "right")
    {
        irrstCstickRead(&stick);

        if (axis == "rightx")
            value = stick.dx;
        else if (axis == "righty")
            value = stick.dy;
    }

    value = value / 156.0f;

    return std::clamp(value, -1.0f, 1.0f);
}

size_t Gamepad::GetID()
{
    return this->id;
}

std::string Gamepad::GetName()
{
    return "Nintendo 3DS";
}

std::pair<float, float> Gamepad::GetVibration()
{
    return std::make_pair(0.0f, 0.0f);
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
            // this is wrong?
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
        if (it->second & Input::GetKeyHeld<u32>())
        {
            if (button == it->first)
                return true;
        }
    }

    return false;
}

bool Gamepad::IsVibrationSupported()
{
    return false;
}

bool Gamepad::SetVibration(float left, float right, float duration)
{
    // do nothing and return false
    return false;
}
