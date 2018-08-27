#include "common/runtime.h"
#include "objects/gamepad/gamepad.h"

#define PAD_MAX 128

Gamepad::Gamepad() : Object("Joystick")
{
    this->id = 1;
}

int Gamepad::GetID()
{
    return this->id;
}

bool Gamepad::IsConnected()
{
    return true;
}

float Gamepad::GetAxis(int axis)
{
    circlePosition circle;
    circlePosition cStick;

    accelVector accelerometer;
    angularRate gyroscope;

    hidCircleRead(&circle);

    if (axis == 1)
        return this->ClampAxis(circle.dx);
    else if (axis == 2)
        return this->ClampAxis(circle.dy);

    if (IR_ENABLED)
    {
        irrstCstickRead(&cStick);

        if (axis == 3)
            return this->ClampAxis(cStick.dx);
        else if (axis == 4)
            return this->ClampAxis(cStick.dy);
    }

    hidGyroRead(&gyroscope);

    if (axis == 5)
        return gyroscope.x;
    else if (axis == 6)
        return gyroscope.y;
    else if (axis == 7)
        return gyroscope.z;

    hidAccelRead(&accelerometer);

    if (axis == 8)
        return accelerometer.x;
    else if (axis == 9)
        return accelerometer.y;
    else if (axis == 10)
        return accelerometer.z;

    return 0.0f;
}

float Gamepad::ClampAxis(float x)
{
    double returnValue = clamp(-PAD_MAX, x, PAD_MAX) / PAD_MAX;

    if (abs(returnValue) < 0.01)
        return 0;
    
    return returnValue;
}

int Gamepad::GetButtonCount()
{
    return KEYS.size() - 9;
}

bool Gamepad::IsVibrationSupported()
{
    return false;
}

string Gamepad::GetName()
{
    return "Nintendo 3DS";
}

bool Gamepad::IsButtonValid(const string & button)
{
    if (button.find("cpad") != button.npos)
        return false;
    else if (button.find("cstick") != button.npos)
        return false;

    return true;
}

string Gamepad::GetInput(u32 key)
{
    for (uint i = 0; i < KEYS.size(); i++)
    {
        if (key & BIT(i))
        {
            if (this->IsButtonValid(KEYS[i]))
                return KEYS[i];
        }
    }

    return "nil";
}

bool Gamepad::IsDown(int button)
{
    hidScanInput();

    u32 heldButton = hidKeysHeld();
    bool keyDown = false;

    for (uint i = 0; i < KEYS.size(); i++)
    {
        if (KEYS[i] != "touch")
        {
            if (heldButton & BIT(i) && button == i)
            {
                keyDown = true;
                break;
            }
        }
    }

    return keyDown;
}

bool Gamepad::IsGamepadDown(const string & button)
{
    hidScanInput();

    u32 heldButton = hidKeysHeld();
    bool keyDown = false;

    for (uint i = 0; i < KEYS.size(); i++)
    {
        if (KEYS[i] != "" && KEYS[i] != "touch")
        {
            if (heldButton & BIT(i) && button == KEYS[i])
            {
                keyDown = true;
                break;
            }
        }
    }

    return keyDown;
}