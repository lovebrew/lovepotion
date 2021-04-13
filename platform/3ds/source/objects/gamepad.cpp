#include "objects/gamepad/gamepad.h"
#include "driver/hidrv.h"

using namespace love;

#include "modules/event/event.h"

#define EVENT_MODULE() (Module::GetInstance<love::Event>(Module::M_EVENT))

Gamepad::Gamepad(size_t id) : common::Gamepad(id)
{}

Gamepad::Gamepad(size_t id, size_t index) : common::Gamepad(id)
{
    this->Open(index);
}

Gamepad::~Gamepad()
{
    this->Close();
}

bool Gamepad::Open(size_t index)
{
    this->Close();

    this->name = "Nintendo 3DS";

    return this->IsConnected();
}

void Gamepad::Close()
{
    this->instanceID = -1;
    this->vibration  = Vibration();
}

bool Gamepad::IsConnected() const
{
    return true;
}

const char* Gamepad::GetName() const
{
    return this->name.c_str();
}

size_t Gamepad::GetAxisCount() const
{
    bool isN3DS = false;
    APT_CheckNew3DS(&isN3DS);

    /* total axes */
    size_t axes = 12;

    if (!isN3DS)
        return axes - 2;

    return axes;
}

size_t Gamepad::GetButtonCount() const
{
    /* constant value anyways */
    return GAMEPAD_BUTTON_MAX_ENUM - 1;
}

float Gamepad::GetAxis(size_t axis) const
{
    if (axis < 0 || axis >= this->GetAxisCount())
        return 0.0f;

    float value = 0.0f;

    if (axis == 1 || axis == 2)
    {
        circlePosition leftStick;
        hidCircleRead(&leftStick);

        if (axis == 1)
            value = leftStick.dx;
        else
            value = -leftStick.dy;

        return Gamepad::ClampValue(value / JOYSTICK_MAX);
    }
    else if (axis == 3 || axis == 4)
    {
        circlePosition rightStick;
        irrstCstickRead(&rightStick);

        if (axis == 3)
            value = rightStick.dx;
        else
            value = -rightStick.dy;

        return Gamepad::ClampValue(value / JOYSTICK_MAX);
    }
    else if (axis == 5)
    {
        //    if (Input::GetKeyHeld<u32>() & KEY_ZL)
        //         return 1.0f;
        //     else if (Input::GetKeyUp<u32>() & KEY_ZL)
        //         return 0.0f;
    }
    else if (axis == 6)
    {
        // if (Input::GetKeyHeld<u32>() & KEY_ZR)
        //     return 1.0f;
        // else if (Input::GetKeyUp<u32>() & KEY_ZR)
        //     return 0.0f;
    }
    else
    {
        if (axis >= 7 && axis < 10)
        {
            angularRate gyroscope;
            hidGyroRead(&gyroscope);

            if (axis == 7)
                return gyroscope.x;
            else if (axis == 8)
                return gyroscope.y;

            return gyroscope.z;
        }
        else if (axis >= 10 && axis < 13)
        {
            accelVector accelerometer;
            hidAccelRead(&accelerometer);

            if (axis == 10)
                return accelerometer.x;
            else if (axis == 11)
                return accelerometer.y;

            return accelerometer.z;
        }
    }

    return 0.0f;
}

std::vector<float> Gamepad::GetAxes() const
{
    std::vector<float> axes;
    size_t count = this->GetAxisCount();

    if (count <= 0)
        return axes;

    axes.reserve(count);

    for (size_t index = 0; index < count; index++)
        axes.push_back(this->GetAxis(index));

    return axes;
}

bool Gamepad::IsDown(const std::vector<size_t>& buttons) const
{
    size_t buttonCount = this->GetButtonCount();

    for (size_t button : buttons)
    {
        if (button < 0 || button >= buttonCount)
            continue;

        return EVENT_MODULE()->GetDriver()->IsDown(button);
    }

    return false;
}

float Gamepad::GetGamepadAxis(Gamepad::GamepadAxis axis) const
{
    const char* name = nullptr;
    if (!common::Gamepad::GetConstant(axis, name))
        return 0.0f;

    switch (axis)
    {
        case GAMEPAD_AXIS_LEFTX:
            return this->GetAxis(1);
        case GAMEPAD_AXIS_LEFTY:
            return this->GetAxis(2);
        case GAMEPAD_AXIS_RIGHTX:
            return this->GetAxis(3);
        case GAMEPAD_AXIS_RIGHTY:
            return this->GetAxis(4);
        default:
            break;
    }

    return 0.0f;
}

bool Gamepad::IsGamepadDown(const std::vector<GamepadButton>& buttons) const
{
    int32_t consoleButton = -1;
    for (GamepadButton button : buttons)
    {
        /* make sure our out button isn't invalid too */
        if (!GetConstant(button, consoleButton) || consoleButton < 0)
            continue;

        if (consoleButton & EVENT_MODULE()->GetDriver()->GetButtonHeld())
            return true;
    }

    return false;
}

bool Gamepad::IsVibrationSupported()
{
    return false;
}

bool Gamepad::SetVibration(float left, float right, float duration)
{
    return false;
}

bool Gamepad::SetVibration()
{
    return false;
}

void Gamepad::GetVibration(float& left, float& right)
{
    left  = 0.0f;
    right = 0.0f;
}

bool Gamepad::GetConstant(int32_t in, Gamepad::GamepadButton& out)
{
    return buttons.Find(in, out);
}

bool Gamepad::GetConstant(Gamepad::GamepadButton in, int32_t& out)
{
    return buttons.Find(in, out);
}

EnumMap<Gamepad::GamepadButton, int32_t, Gamepad::GAMEPAD_BUTTON_MAX_ENUM>::Entry
    Gamepad::buttonEntries[] = {
        { Gamepad::GAMEPAD_BUTTON_A, KEY_A },
        { Gamepad::GAMEPAD_BUTTON_B, KEY_B },
        { Gamepad::GAMEPAD_BUTTON_X, KEY_X },
        { Gamepad::GAMEPAD_BUTTON_Y, KEY_Y },
        { Gamepad::GAMEPAD_BUTTON_BACK, KEY_SELECT },
        { Gamepad::GAMEPAD_BUTTON_GUIDE, -1 },
        { Gamepad::GAMEPAD_BUTTON_START, KEY_START },
        { Gamepad::GAMEPAD_BUTTON_LEFTSTICK, -1 },  //< left stick click doesn't exist
        { Gamepad::GAMEPAD_BUTTON_RIGHTSTICK, -1 }, //< right stick click doesn't exist
        { Gamepad::GAMEPAD_BUTTON_LEFTSHOULDER, KEY_L },
        { Gamepad::GAMEPAD_BUTTON_RIGHTSHOULDER, KEY_R },
        { Gamepad::GAMEPAD_BUTTON_DPAD_UP, KEY_DUP },
        { Gamepad::GAMEPAD_BUTTON_DPAD_DOWN, KEY_DDOWN },
        { Gamepad::GAMEPAD_BUTTON_DPAD_LEFT, KEY_DLEFT },
        { Gamepad::GAMEPAD_BUTTON_DPAD_RIGHT, KEY_DRIGHT },
    };

EnumMap<Gamepad::GamepadButton, int32_t, Gamepad::GAMEPAD_BUTTON_MAX_ENUM> Gamepad::buttons(
    Gamepad::buttonEntries, sizeof(Gamepad::buttonEntries));