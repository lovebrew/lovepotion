#include "common/runtime.h"
#include "objects/gamepad/gamepad.h"

#include "common/backend/input.h"

using namespace love;

Gamepad::Gamepad(size_t id) : common::Gamepad(id),
                              handles(nullptr)
{}

Gamepad::Gamepad(size_t id, size_t index) : common::Gamepad(id),
                                            handles(nullptr)
{
    this->Open(index);
}

Gamepad::~Gamepad()
{
    this->Close();
}

PadState & Gamepad::GetPadState()
{
    return this->pad;
}

bool Gamepad::Open(size_t index)
{
    this->Close();

    padInitializeDefault(&this->pad);
    padUpdate(&this->pad);

    this->style = padGetStyleSet(&this->pad);

    if (style & HidNpadStyleTag_NpadFullKey)
    {
        this->name = "Pro Controller";

        this->handles = new HidSixAxisSensorHandle[1];
        hidGetSixAxisSensorHandles(&this->handles[0], 1, static_cast<HidNpadIdType>(this->id), HidNpadStyleTag_NpadFullKey);
    }
    else if (style & HidNpadStyleTag_NpadHandheld)
    {
        this->name = "Nintendo Switch";

        this->handles = new HidSixAxisSensorHandle[1];
        hidGetSixAxisSensorHandles(&this->handles[0], 1, static_cast<HidNpadIdType>(this->id), HidNpadStyleTag_NpadHandheld);
    }
    else if (style & HidNpadStyleTag_NpadJoyDual)
    {
        this->name = "Dual Joy-Con";

        this->handles = new HidSixAxisSensorHandle[2];
        hidGetSixAxisSensorHandles(&this->handles[0], 2, static_cast<HidNpadIdType>(this->id), HidNpadStyleTag_NpadJoyDual);
    }

    hidStartSixAxisSensor(this->handles[0]);

    u32 attributes = padGetAttributes(&this->pad);
    if (this->style & HidNpadStyleTag_NpadJoyDual && attributes & HidNpadAttribute_IsRightConnected)
        hidStartSixAxisSensor(this->handles[1]);

    return this->IsConnected();
}

void Gamepad::Close()
{
    this->instanceID = -1;
    this->vibration = Vibration();

    if (handles != nullptr)
    {
        hidStopSixAxisSensor(this->handles[0]);

        u32 attributes = padGetAttributes(&this->pad);
        if (this->style & HidNpadStyleTag_NpadJoyDual && attributes & HidNpadAttribute_IsRightConnected)
            hidStopSixAxisSensor(this->handles[1]);
    }

    delete [] this->handles;
}

bool Gamepad::IsConnected() const
{
    return padIsConnected(&this->pad);
}

const char * Gamepad::GetName() const
{
    return this->name.c_str();
}

size_t Gamepad::GetAxisCount() const
{
    return this->IsConnected() ? 12 : 0;
}

size_t Gamepad::GetButtonCount() const
{
    return this->IsConnected() ? GAMEPAD_BUTTON_MAX_ENUM - 1 : 0;
}

float Gamepad::GetAxis(size_t axis) const
{
    if (!this->IsConnected())
        return 0.0f;

    float value = 0.0f;

    if (axis == 1 || axis == 2)
    {
        HidAnalogStickState left = padGetStickPos(&pad, 0);

        if (axis == 1)
            value = left.x;
        else if (axis == 2)
            value = -left.y;

        return Gamepad::ClampValue(value / JOYSTICK_MAX);
    }
    else if (axis == 3 || axis == 4)
    {
        HidAnalogStickState right = padGetStickPos(&pad, 1);

        if (axis == 3)
            value = right.x;
        else if (axis == 4)
            value = -right.y;

        return Gamepad::ClampValue(value / JOYSTICK_MAX);
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

        HidSixAxisSensorState sixAxisState;

        if (this->style & HidNpadStyleTag_NpadFullKey)
            hidGetSixAxisSensorStates(this->handles[0], &sixAxisState, 1);
        else if (this->style & HidNpadStyleTag_NpadHandheld)
            hidGetSixAxisSensorStates(this->handles[0], &sixAxisState, 1);
        else if (this->style & HidNpadStyleTag_NpadJoyDual)
        {
            /*
            ** For JoyDual, read from either the Left or Right Joy-Con
            ** depending on which is/are connected
            */

            u32 attributes = padGetAttributes(&pad);
            if (attributes & HidNpadAttribute_IsLeftConnected)
                hidGetSixAxisSensorStates(handles[0], &sixAxisState, 1);
            else if (attributes & HidNpadAttribute_IsRightConnected)
                hidGetSixAxisSensorStates(handles[1], &sixAxisState, 1);
        }

        if (axis >= 7 and axis < 10)
        {
            if (axis == 7)
                return sixAxisState.angular_velocity.x;
            else if (axis == 8)
                return sixAxisState.angular_velocity.y;

            return sixAxisState.angular_velocity.z;
        }
        else if (axis >= 10 and axis < 13)
        {
            if (axis == 7)
                return sixAxisState.acceleration.x;
            else if (axis == 8)
                return sixAxisState.acceleration.y;

            return sixAxisState.acceleration.z;
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

bool Gamepad::IsDown(const std::vector<size_t> & buttons) const
{
    size_t buttonCount = this->GetButtonCount();
    const auto consoleButtons = Input::buttons;

    for (size_t button : buttons)
    {
        if (button < 0 || button >= buttonCount)
            continue;

        for (auto it = consoleButtons.begin(); it != consoleButtons.end(); it++)
        {
            if (it->second & Input::GetKeyHeld<u32>())
            {
                size_t index = std::distance(it, consoleButtons.begin()) - 1;
                return button == index;
            }
        }
    }

    return false;
}

float Gamepad::GetGamepadAxis(Gamepad::GamepadAxis axis) const
{
    const char * name = nullptr;
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

bool Gamepad::IsGamepadDown(const std::vector<GamepadButton> & buttons) const
{
    int32_t consoleButton = -1;
    for (GamepadButton button : buttons)
    {
        /* make sure our out button isn't invalid too */
        if (!GetConstant(button, consoleButton) || consoleButton < 0)
            continue;

        if (consoleButton & Input::GetKeyHeld<u32>())
            return true;
    }

    return false;
}

std::string Gamepad::GetGUID() const
{
    return this->guid;
}

size_t Gamepad::GetInstanceID() const
{
    return this->instanceID;
}

size_t Gamepad::GetID() const
{
    return this->id;
}

bool Gamepad::IsVibrationSupported()
{
    return true;
}

bool Gamepad::SetVibration(float left, float right, float duration)
{
    return false;
}

bool Gamepad::SetVibration()
{
    return false;
}

void Gamepad::GetVibration(float & left, float & right)
{
    left = 0.0f;
    right = 0.0f;
}

bool Gamepad::GetConstant(int32_t in, Gamepad::GamepadButton & out)
{
    return buttons.Find(in, out);
}

bool Gamepad::GetConstant(Gamepad::GamepadButton in, int32_t & out)
{
    return buttons.Find(in, out);
}

EnumMap<Gamepad::GamepadButton, int32_t, Gamepad::GAMEPAD_BUTTON_MAX_ENUM>::Entry Gamepad::buttonEntries[] =
{
    { Gamepad::GAMEPAD_BUTTON_A,             HidNpadButton_A      },
    { Gamepad::GAMEPAD_BUTTON_B,             HidNpadButton_B      },
    { Gamepad::GAMEPAD_BUTTON_X,             HidNpadButton_X      },
    { Gamepad::GAMEPAD_BUTTON_Y,             HidNpadButton_Y      },
    { Gamepad::GAMEPAD_BUTTON_BACK,          HidNpadButton_Minus  },
    { Gamepad::GAMEPAD_BUTTON_GUIDE,         -1                   },
    { Gamepad::GAMEPAD_BUTTON_START,         HidNpadButton_Plus   },
    { Gamepad::GAMEPAD_BUTTON_LEFTSTICK,     HidNpadButton_StickL }, //< left stick click doesn't exist
    { Gamepad::GAMEPAD_BUTTON_RIGHTSTICK,    HidNpadButton_StickR }, //< right stick click doesn't exist
    { Gamepad::GAMEPAD_BUTTON_LEFTSHOULDER,  HidNpadButton_L      },
    { Gamepad::GAMEPAD_BUTTON_RIGHTSHOULDER, HidNpadButton_R      },
    { Gamepad::GAMEPAD_BUTTON_DPAD_UP,       HidNpadButton_Up     },
    { Gamepad::GAMEPAD_BUTTON_DPAD_DOWN,     HidNpadButton_Down   },
    { Gamepad::GAMEPAD_BUTTON_DPAD_LEFT,     HidNpadButton_Left   },
    { Gamepad::GAMEPAD_BUTTON_DPAD_RIGHT,    HidNpadButton_Right  },
};

EnumMap<Gamepad::GamepadButton, int32_t, Gamepad::GAMEPAD_BUTTON_MAX_ENUM> Gamepad::buttons(Gamepad::buttonEntries, sizeof(Gamepad::buttonEntries));