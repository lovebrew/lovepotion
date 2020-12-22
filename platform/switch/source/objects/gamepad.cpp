#include "common/runtime.h"
#include "objects/gamepad/gamepad.h"

#include "common/backend/input.h"
#include "modules/timer/timer.h"

#include "modules/joystick/joystick.h"

using namespace love;

#define JOYSTICK_MODULE() (Module::GetInstance<Joystick>(Module::M_JOYSTICK))

Gamepad::Gamepad(size_t id) : common::Gamepad(id),
                              sixAxisHandles(nullptr),
                              vibrationHandles(nullptr)
{}

Gamepad::Gamepad(size_t id, size_t index) : common::Gamepad(id),
                                            sixAxisHandles(nullptr),
                                            vibrationHandles(nullptr)
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

const HidNpadStyleTag Gamepad::GetStyleTag() const
{
    u32 tagStyle = padGetStyleSet(&this->pad);

    if (tagStyle & HidNpadStyleTag_NpadFullKey)
        return HidNpadStyleTag_NpadFullKey;
    else if (tagStyle & HidNpadStyleTag_NpadHandheld)
        return HidNpadStyleTag_NpadHandheld;
    else if (tagStyle & HidNpadStyleTag_NpadJoyDual)
        return HidNpadStyleTag_NpadJoyDual;

    return HidNpadStyleTag::HidNpadStyleTag_NpadSystem;
}

const HidNpadIdType Gamepad::GetNpadIdType() const
{
    if (padIsHandheld(&this->pad))
        return HidNpadIdType_Handheld;

    return static_cast<HidNpadIdType>(this->id);
}

bool Gamepad::Open(size_t index)
{
    this->Close();

    HidNpadIdType idType = this->GetNpadIdType();

    if (index == 0)
        padInitializeDefault(&this->pad);
    else
        padInitialize(&this->pad, static_cast<HidNpadIdType>(HidNpadIdType_No1 + index));

    padUpdate(&this->pad);

    HidNpadStyleTag styleTag = this->GetStyleTag();

    this->style = padGetStyleSet(&this->pad);

    switch (styleTag)
    {
        case HidNpadStyleTag_NpadFullKey:
            this->name = "Pro Controller";

            this->sixAxisHandles = std::make_unique<HidSixAxisSensorHandle[]>(1);
            hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 1, static_cast<HidNpadIdType>(this->id), HidNpadStyleTag_NpadFullKey);

            this->vibrationHandles = std::make_unique<HidVibrationDeviceHandle[]>(1);
            break;
        case HidNpadStyleTag_NpadHandheld:
            this->name = "Nintendo Switch";

            this->sixAxisHandles = std::make_unique<HidSixAxisSensorHandle[]>(1);
            hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 1, HidNpadIdType_Handheld, HidNpadStyleTag_NpadHandheld);

            this->vibrationHandles = std::make_unique<HidVibrationDeviceHandle[]>(2);
            break;
        case HidNpadStyleTag_NpadJoyDual:
            this->name = "Dual Joy-Con";

            this->sixAxisHandles = std::make_unique<HidSixAxisSensorHandle[]>(2);
            hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 2, static_cast<HidNpadIdType>(this->id), HidNpadStyleTag_NpadJoyDual);

            this->vibrationHandles = std::make_unique<HidVibrationDeviceHandle[]>(2);
        default:
            break;
    }

    memset(this->vibrationValues, 0, sizeof(this->vibrationValues));

    size_t handleCount = (styleTag == HidNpadStyleTag_NpadFullKey) ? 1 : 2;
    hidInitializeVibrationDevices(this->vibrationHandles.get(), handleCount, this->GetNpadIdType(), styleTag);

    hidStartSixAxisSensor(this->sixAxisHandles[0]);

    u32 attributes = padGetAttributes(&this->pad);
    if ((this->style & HidNpadStyleTag_NpadJoyDual) && (attributes & HidNpadAttribute_IsRightConnected))
        hidStartSixAxisSensor(this->sixAxisHandles[1]);

    return this->IsConnected();
}

void Gamepad::Close()
{
    this->instanceID = -1;

    /* stop vibration, just in case */
    if (this->vibrationHandles != nullptr)
        this->SetVibration(0, 0);

    this->vibration = Vibration();

    if (this->sixAxisHandles != nullptr)
    {
        hidStopSixAxisSensor(this->sixAxisHandles[0]);

        u32 attributes = padGetAttributes(&this->pad);
        if ((this->style & HidNpadStyleTag_NpadJoyDual) && (attributes & HidNpadAttribute_IsRightConnected))
            hidStopSixAxisSensor(this->sixAxisHandles[1]);
    }

    this->sixAxisHandles = nullptr;
    this->vibrationHandles = nullptr;
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

        HidSixAxisSensorState sixAxisState = { 0 };

        if (this->style & HidNpadStyleTag_NpadFullKey)
            hidGetSixAxisSensorStates(this->sixAxisHandles[0], &sixAxisState, 1);
        else if (this->style & HidNpadStyleTag_NpadHandheld)
            hidGetSixAxisSensorStates(this->sixAxisHandles[0], &sixAxisState, 1);
        else if (this->style & HidNpadStyleTag_NpadJoyDual)
        {
            /*
            ** For JoyDual, read from either the Left or Right Joy-Con
            ** depending on which is/are connected
            */

            u32 attributes = padGetAttributes(&pad);
            if (attributes & HidNpadAttribute_IsLeftConnected)
                hidGetSixAxisSensorStates(sixAxisHandles[0], &sixAxisState, 1);
            else if (attributes & HidNpadAttribute_IsRightConnected)
                hidGetSixAxisSensorStates(sixAxisHandles[1], &sixAxisState, 1);
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
    left = std::clamp(left, 0.0f, 1.0f);
    right = std::clamp(right, 0.0f, 1.0f);

    uint32_t length = Vibration::max;
    if (duration >= 0)
        length = (uint32_t)std::min(duration, Vibration::max / 1000.0f);

    for (auto & vibrationValue : this->vibrationValues)
    {
        vibrationValue.freq_low  = 160.0f;
        vibrationValue.freq_high = 320.0f;
    }

    this->vibrationValues[0].amp_low  = left;
    this->vibrationValues[0].amp_high = right;

    this->vibrationValues[1].amp_low  = left;
    this->vibrationValues[1].amp_high = right;

    Result res = hidSendVibrationValues(this->vibrationHandles.get(), this->vibrationValues, 2);
    bool success = R_SUCCEEDED(res);

    if (success)
    {
        this->vibration.left = left;
        this->vibration.right = right;

        if (left != 0 && right != 0)
            this->vibration.id = this->id;

        if (length == Vibration::max)
            this->vibration.endTime = Vibration::max;
        else
            this->vibration.endTime = love::Timer::GetTime() + length;

        JOYSTICK_MODULE()->AddVibration(this, this->id);
    }
    else
    {
        this->vibration.left = this->vibration.right = 0.0f;
        this->vibration.endTime = Vibration::max;
    }

    return R_SUCCEEDED(success);
}

bool Gamepad::SetVibration()
{
    this->vibration.id = -1;
    return this->SetVibration(0, 0);
}

void Gamepad::GetVibration(float & left, float & right)
{
    left = this->vibration.left;
    right = this->vibration.right;
}

const Gamepad::Vibration & Gamepad::GetVibration() const
{
    return this->vibration;
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