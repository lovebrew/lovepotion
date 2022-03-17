#include "objects/gamepad/gamepad.h"

#include "driver/hidrv.h"
#include "modules/timer/timer.h"

#include "common/bidirectionalmap.h"
#include "modules/event/event.h"
#include "modules/joystick/joystick.h"

using namespace love;

#define JOYSTICK_MODULE() (Module::GetInstance<Joystick>(Module::M_JOYSTICK))

#define INVALID_GAMEPAD_BUTTON static_cast<Gamepad::GamepadButton>(-1)
#define INVALID_NPAD_BUTTON    static_cast<HidNpadButton>(0)
static constexpr HidNpadStyleTag INVALID_STYLE_TAG = static_cast<HidNpadStyleTag>(-1);

Gamepad::Gamepad(size_t id) :
    common::Gamepad(id),
    sixAxisHandles(nullptr),
    vibrationHandles(nullptr),
    buttonStates()
{}

Gamepad::Gamepad(size_t id, size_t index) :
    common::Gamepad(id),
    sixAxisHandles(nullptr),
    vibrationHandles(nullptr),
    buttonStates()
{
    this->Open(index);
}

Gamepad::~Gamepad()
{
    this->Close();
}

void Gamepad::UpdatePadState()
{
    padUpdate(&this->pad);

    this->buttonStates.pressed  = padGetButtonsDown(&this->pad);
    this->buttonStates.released = padGetButtonsUp(&this->pad);
}

HidNpadStyleTag Gamepad::GetStyleTag()
{
    uint32_t styleSet = padGetStyleSet(&this->pad);

    if (styleSet & HidNpadStyleTag_NpadFullKey)
        return HidNpadStyleTag_NpadFullKey;
    else if (styleSet & HidNpadStyleTag_NpadHandheld)
        return HidNpadStyleTag_NpadHandheld;
    else if (styleSet & HidNpadStyleTag_NpadJoyDual)
        return HidNpadStyleTag_NpadJoyDual;
    else if (styleSet & HidNpadStyleTag_NpadJoyLeft)
        return HidNpadStyleTag_NpadJoyLeft;
    else if (styleSet & HidNpadStyleTag_NpadJoyRight)
        return HidNpadStyleTag_NpadJoyRight;

    return INVALID_STYLE_TAG;
}

HidNpadIdType Gamepad::GetNpadIdType()
{
    return static_cast<HidNpadIdType>(HidNpadIdType_No1 + this->id);
}

bool Gamepad::Open(size_t index)
{
    this->Close();

    if (index == 0)
        padInitializeDefault(&this->pad);
    else
        padInitialize(&this->pad, this->GetNpadIdType());

    padUpdate(&this->pad);

    HidNpadStyleTag styleTag = this->GetStyleTag();

    if (styleTag == INVALID_STYLE_TAG)
        return false;

    this->style = padGetStyleSet(&this->pad);

    switch (styleTag)
    {
        case HidNpadStyleTag_NpadFullKey:
            this->name = "Pro Controller";

            this->sixAxisHandles = std::make_unique<HidSixAxisSensorHandle[]>(1);
            hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 1,
                                       static_cast<HidNpadIdType>(this->id),
                                       HidNpadStyleTag_NpadFullKey);

            this->vibrationHandles = std::make_unique<HidVibrationDeviceHandle[]>(1);

            break;
        case HidNpadStyleTag_NpadJoyLeft:
            this->name = "Left Joy-Con";

            this->sixAxisHandles = std::make_unique<HidSixAxisSensorHandle[]>(1);
            hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 1,
                                       static_cast<HidNpadIdType>(this->id),
                                       HidNpadStyleTag_NpadFullKey);

            this->vibrationHandles = std::make_unique<HidVibrationDeviceHandle[]>(1);

            break;
        case HidNpadStyleTag_NpadJoyRight:
            this->name = "Right Joy-Con";

            this->sixAxisHandles = std::make_unique<HidSixAxisSensorHandle[]>(1);
            hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 1,
                                       static_cast<HidNpadIdType>(this->id),
                                       HidNpadStyleTag_NpadFullKey);

            this->vibrationHandles = std::make_unique<HidVibrationDeviceHandle[]>(1);

            break;
        case HidNpadStyleTag_NpadHandheld:
            this->name = "Nintendo Switch";

            this->sixAxisHandles = std::make_unique<HidSixAxisSensorHandle[]>(1);
            hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 1, HidNpadIdType_Handheld,
                                       HidNpadStyleTag_NpadHandheld);

            this->vibrationHandles = std::make_unique<HidVibrationDeviceHandle[]>(2);

            break;
        case HidNpadStyleTag_NpadJoyDual:
            this->name = "Dual Joy-Con";

            this->sixAxisHandles = std::make_unique<HidSixAxisSensorHandle[]>(2);
            hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 2,
                                       static_cast<HidNpadIdType>(this->id),
                                       HidNpadStyleTag_NpadJoyDual);

            this->vibrationHandles = std::make_unique<HidVibrationDeviceHandle[]>(2);

            break;
        default:
            /* not supported */
            return false;
    }

    memset(this->vibrationValues, 0, sizeof(this->vibrationValues));

    size_t handleCount = (styleTag == HidNpadStyleTag_NpadFullKey) ? 1 : 2;
    hidInitializeVibrationDevices(this->vibrationHandles.get(), handleCount, this->GetNpadIdType(),
                                  styleTag);

    hidStartSixAxisSensor(this->sixAxisHandles[0]);

    u32 attributes = padGetAttributes(&this->pad);
    if ((this->style & HidNpadStyleTag_NpadJoyDual) &&
        (attributes & HidNpadAttribute_IsRightConnected))
        hidStartSixAxisSensor(this->sixAxisHandles[1]);

    this->guid = std::to_string(this->style);

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
        if ((this->style & HidNpadStyleTag_NpadJoyDual) &&
            (attributes & HidNpadAttribute_IsRightConnected))
            hidStopSixAxisSensor(this->sixAxisHandles[1]);
    }

    this->sixAxisHandles   = nullptr;
    this->vibrationHandles = nullptr;
}

bool Gamepad::IsConnected() const
{
    return padIsConnected(&this->pad);
}

const char* Gamepad::GetName() const
{
    return this->name.c_str();
}

size_t Gamepad::GetAxisCount() const
{
    return this->IsConnected() ? 12 : 0;
}

size_t Gamepad::GetButtonCount() const
{
    return this->IsConnected() ? 16 - 6 : 0;
}

float Gamepad::GetAxis(size_t axis) const
{
    if (!this->IsConnected())
        return 0.0f;

    float value = 0.0f;

    if (axis == 1 || axis == 2)
    {
        HidAnalogStickState left = padGetStickPos(&this->pad, 0);

        if (axis == 1)
            value = left.x;
        else if (axis == 2)
            value = -left.y;

        return Gamepad::ClampValue(value / JOYSTICK_MAX);
    }
    else if (axis == 3 || axis == 4)
    {
        HidAnalogStickState right = padGetStickPos(&this->pad, 1);

        if (axis == 3)
            value = right.x;
        else if (axis == 4)
            value = -right.y;

        return Gamepad::ClampValue(value / JOYSTICK_MAX);
    }
    else if (axis == 5)
    {
        if (padGetButtons(&this->pad) & HidNpadButton_ZL)
            return 1.0f;

        return 0.0f;
    }
    else if (axis == 6)
    {
        if (padGetButtons(&this->pad) & HidNpadButton_ZR)
            return 1.0f;

        return 0.0f;
    }
    else
    {
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

/* helper functions */
bool Gamepad::IsDown(size_t index, ButtonMapping& button)
{
    if (!this->IsConnected())
        return false;

    HidNpadButton hidButton = INVALID_NPAD_BUTTON;

    if (!this->buttonStates.pressed)
        return false;

    const auto records = buttons.GetEntries().first;

    hidButton = static_cast<HidNpadButton>(records[index].second);

    if (hidButton & this->buttonStates.pressed)
    {
        this->buttonStates.pressed ^= hidButton;
        button = std::make_pair(records[index].first, index);

        return true;
    }

    return false;
}

bool Gamepad::IsUp(size_t index, ButtonMapping& button)
{
    if (!this->IsConnected())
        return false;

    HidNpadButton hidButton = INVALID_NPAD_BUTTON;

    if (!this->buttonStates.released)
        return false;

    const auto records = buttons.GetEntries().first;

    hidButton = static_cast<HidNpadButton>(records[index].second);

    if (hidButton & this->buttonStates.released)
    {
        this->buttonStates.released ^= hidButton;
        button = std::make_pair(records[index].first, index);

        return true;
    }

    return false;
}

bool Gamepad::IsHeld(size_t index, ButtonMapping& button) const
{
    if (!this->IsConnected())
        return false;

    uint64_t heldSet = padGetButtons(&this->pad);
    HidNpadButton hidButton;

    auto recordPair = buttons.GetEntries();
    auto records    = recordPair.first;

    for (size_t i = 0; i < recordPair.second; i++)
    {
        if ((hidButton = static_cast<HidNpadButton>(records[i].second)) & heldSet)
        {
            button = { records[i].first, i };
            break;
        }
    }

    return (heldSet & hidButton);
}

bool Gamepad::IsDown(const std::vector<size_t>& buttonsVector) const
{
    auto recordPair = buttons.GetEntries();

    uint32_t heldSet = padGetButtons(&this->pad);
    auto records     = recordPair.first;

    for (size_t button : buttonsVector)
    {
        if (button < 0 || button >= recordPair.second)
            continue;

        if (heldSet & static_cast<HidNpadButton>(records[button].second))
            return true;
    }

    return false;
}

bool Gamepad::IsGamepadDown(const std::vector<GamepadButton>& buttonsVector) const
{
    uint64_t heldSet = padGetButtons(&this->pad);

    GamepadButton consoleButton = INVALID_GAMEPAD_BUTTON;
    const char* name            = nullptr;

    for (GamepadButton button : buttonsVector)
    {
        /* make sure our out button isn't invalid */
        if (!GetConstant(button, name))
            continue;

        /* convert to the proper button */
        if (!GetConstant(name, consoleButton))
            continue;

        if (heldSet & static_cast<HidNpadButton>(consoleButton))
            return true;
    }

    return false;
}

float Gamepad::GetGamepadAxis(GamepadAxis axis) const
{
    const char* name = nullptr;
    if (!Gamepad::GetConstant(axis, name))
        return 0.0f;

    switch (axis)
    {
        case GamepadAxis::GAMEPAD_AXIS_LEFTX:
            return this->GetAxis(1);
        case GamepadAxis::GAMEPAD_AXIS_LEFTY:
            return this->GetAxis(2);
        case GamepadAxis::GAMEPAD_AXIS_RIGHTX:
            return this->GetAxis(3);
        case GamepadAxis::GAMEPAD_AXIS_RIGHTY:
            return this->GetAxis(4);
        case GamepadAxis::GAMEPAD_AXIS_TRIGGERLEFT:
            return this->GetAxis(5);
        case GamepadAxis::GAMEPAD_AXIS_TRIGGERRIGHT:
            return this->GetAxis(6);
        default:
            break;
    }

    return 0.0f;
}

bool Gamepad::IsVibrationSupported()
{
    return true;
}

bool Gamepad::SetVibration(float left, float right, float duration)
{
    left  = std::clamp(left, 0.0f, 1.0f);
    right = std::clamp(right, 0.0f, 1.0f);

    uint32_t length = Vibration::max;
    if (duration >= 0)
        length = (uint32_t)std::min(duration, Vibration::max / 1000.0f);

    for (auto& vibrationValue : this->vibrationValues)
    {
        vibrationValue.freq_low  = 160.0f;
        vibrationValue.freq_high = 320.0f;
    }

    this->vibrationValues[0].amp_low  = left;
    this->vibrationValues[0].amp_high = right;

    this->vibrationValues[1].amp_low  = left;
    this->vibrationValues[1].amp_high = right;

    Result res   = hidSendVibrationValues(this->vibrationHandles.get(), this->vibrationValues, 2);
    bool success = R_SUCCEEDED(res);

    if (success && (left != 0.0f && right != 0.0f))
    {
        this->vibration.left  = left;
        this->vibration.right = right;

        if (length == Vibration::max)
            this->vibration.endTime = Vibration::max;
        else
            this->vibration.endTime = love::Timer::GetTime() + length;

        JOYSTICK_MODULE()->AddVibration(this, this->id);
    }
    else
    {
        this->vibration.left = this->vibration.right = 0.0f;
        this->vibration.endTime                      = Vibration::max;
    }

    return R_SUCCEEDED(success);
}

bool Gamepad::SetVibration()
{
    this->vibration.id = -1;
    return this->SetVibration(0, 0);
}

void Gamepad::GetVibration(float& left, float& right)
{
    left  = this->vibration.left;
    right = this->vibration.right;
}

const Gamepad::Vibration& Gamepad::GetVibration() const
{
    return this->vibration;
}

bool Gamepad::GetConstant(const char* in, Gamepad::GamepadAxis& out)
{
    return axes.Find(in, out);
}

bool Gamepad::GetConstant(Gamepad::GamepadAxis in, const char*& out)
{
    return axes.ReverseFind(in, out);
}

bool Gamepad::GetConstant(const char* in, GamepadButton& out)
{
    return buttons.Find(in, out);
}

bool Gamepad::GetConstant(GamepadButton in, const char*& out)
{
    return buttons.ReverseFind(in, out);
}
