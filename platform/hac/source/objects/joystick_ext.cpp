#include <objects/joystick_ext.hpp>
#include <utilities/npad.hpp>

#include <modules/joystickmodule_ext.hpp>
#include <modules/timer_ext.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

using namespace love;

#define Module() (Module::GetInstance<JoystickModule<Console::HAC>>(Module::M_JOYSTICK))

template<>
Type Joystick<>::type("Joystick", &Object::type);

// clang-format off
constexpr BidirectionalMap buttons = {
    Joystick<>::GAMEPAD_BUTTON_A,             HidNpadButton_A,
    Joystick<>::GAMEPAD_BUTTON_B,             HidNpadButton_B,
    Joystick<>::GAMEPAD_BUTTON_X,             HidNpadButton_X,
    Joystick<>::GAMEPAD_BUTTON_Y,             HidNpadButton_Y,
    Joystick<>::GAMEPAD_BUTTON_BACK,          HidNpadButton_Minus,
    Joystick<>::GAMEPAD_BUTTON_GUIDE,         (HidNpadButton)-1,
    Joystick<>::GAMEPAD_BUTTON_START,         HidNpadButton_Plus,
    Joystick<>::GAMEPAD_BUTTON_LEFTSHOULDER,  HidNpadButton_L,
    Joystick<>::GAMEPAD_BUTTON_RIGHTSHOULDER, HidNpadButton_R,
    Joystick<>::GAMEPAD_BUTTON_LEFTSTICK,     HidNpadButton_StickL,
    Joystick<>::GAMEPAD_BUTTON_RIGHTSTICK,    HidNpadButton_StickR,
    Joystick<>::GAMEPAD_BUTTON_DPAD_UP,       HidNpadButton_Up,
    Joystick<>::GAMEPAD_BUTTON_DPAD_DOWN,     HidNpadButton_Down,
    Joystick<>::GAMEPAD_BUTTON_DPAD_RIGHT,    HidNpadButton_Right,
    Joystick<>::GAMEPAD_BUTTON_DPAD_LEFT,     HidNpadButton_Left
};
// clang-format on

static bool getConstant(Joystick<>::GamepadButton in, HidNpadButton& out)
{
    return buttons.Find(in, out);
}

static bool getConstant(HidNpadButton in, Joystick<>::GamepadButton& out)
{
    return buttons.ReverseFind(in, out);
}

Joystick<Console::HAC>::Joystick(int id) : state {}, buttonStates {}
{
    this->instanceId = -1;
    this->id         = id;
}

Joystick<Console::HAC>::Joystick(int id, int index) : Joystick(id)
{
    this->Open(index);
}

Joystick<Console::HAC>::~Joystick()
{
    this->Close();
}

bool Joystick<Console::HAC>::Open(int index)
{
    this->Close();

    if (index == 0)
        padInitializeDefault(&this->state);
    else
        padInitialize(&this->state, (HidNpadIdType)index);

    padUpdate(&this->state);
    this->style = npad::GetStyleTag(&this->state);

    this->instanceId = index;

    if (this->style == npad::INVALID_STYLE_TAG)
        return false;

    this->playerId = (HidNpadIdType)index;

    this->guid = guid::GetGamepadGUID(this->GetGamepadType());
    this->name = guid::GetGamepadName(this->GetGamepadType());

    this->sixAxis   = std::move(::SixAxis(this->playerId, this->style));
    this->vibration = std::move(::Vibration(this->playerId, this->style));

    return this->IsConnected();
}

void Joystick<Console::HAC>::Close()
{
    this->instanceId = -1;
    this->playerId   = npad::INVALID_PLAYER_ID;
    this->state      = PadState {};

    this->vibration.SendValues(0, 0);
    this->sixAxis.Stop();
}

guid::GamepadType Joystick<Console::HAC>::GetGamepadType() const
{
    guid::GamepadType type;
    npad::GetConstant(this->style, type);

    return type;
}

void Joystick<Console::HAC>::GetDeviceInfo(int& vendor, int& product, int& version)
{
    guid::DeviceInfo info {};

    if (!guid::GetDeviceInfo(this->GetGamepadType(), info))
        return;

    vendor  = info.vendorId;
    product = info.productId;
    version = info.productVersion;
}

bool Joystick<Console::HAC>::IsConnected() const
{
    return padIsConnected(&this->state);
}

int Joystick<Console::HAC>::GetAxisCount() const
{
    if (!this->IsConnected())
        return 0;

    return guid::GetGamepadAxisCount(this->GetGamepadType());
}

int Joystick<Console::HAC>::GetButtonCount() const
{
    if (!this->IsConnected())
        return 0;

    return guid::GetGamepadButtonCount(this->GetGamepadType());
}

void Joystick<Console::HAC>::Update()
{
    padUpdate(&this->state);

    this->buttonStates.pressed  = padGetButtonsDown(&this->state);
    this->buttonStates.released = padGetButtonsUp(&this->state);
}

bool Joystick<Console::HAC>::IsDown(JoystickInput& result)
{
    if (!this->IsConnected())
        return false;

    if (!this->buttonStates.pressed)
        return false;

    HidNpadButton button = (HidNpadButton)-1;

    const auto entries = buttons.GetEntries();

    for (size_t index = 0; index < entries.second; index++)
    {
        button = (HidNpadButton)entries.first[index].second;

        if (entries.first[index].second == -1)
            continue;

        if (button & this->buttonStates.pressed)
        {
            this->buttonStates.pressed ^= button;
            result = { .type         = InputType::INPUT_TYPE_BUTTON,
                       .button       = entries.first[index].first,
                       .buttonNumber = (int)index };

            return true;
        }
    }

    return false;
}

bool Joystick<Console::HAC>::IsUp(JoystickInput& result)
{
    if (!this->IsConnected())
        return false;

    HidNpadButton button = (HidNpadButton)-1;

    if (!this->buttonStates.released)
        return false;

    const auto entries = buttons.GetEntries();

    for (size_t index = 0; index < entries.second; index++)
    {
        button = (HidNpadButton)entries.first[index].second;

        if (entries.first[index].second == -1)
            continue;

        if (button & this->buttonStates.released)
        {
            this->buttonStates.released ^= button;
            result = { .type         = InputType::INPUT_TYPE_BUTTON,
                       .button       = entries.first[index].first,
                       .buttonNumber = (int)index };

            return true;
        }
    }

    return false;
}

/* helper functionality */

static float getStickPosition(PadState& state, bool horizontal, bool isLeft)
{
    auto stickState = padGetStickPos(&state, isLeft);

    float value = (horizontal) ? stickState.x : stickState.y;
    return std::clamp<float>(value / Joystick<>::JoystickMax, -1.0f, 1.0f);
}

static float getTrigger(uint64_t held, HidNpadButton trigger)
{
    if (held & trigger)
        return 1.0f;

    return 0.0f;
}

static float getHapticAngle(std::vector<Vector3> haptic, size_t index, ::SixAxis::Axis axis)
{
    switch (axis)
    {
        case ::SixAxis::SIXAXIS_X:
            return haptic[index].x;
        case ::SixAxis::SIXAXIS_Y:
            return haptic[index].y;
        case ::SixAxis::SIXAXIS_Z:
            return haptic[index].z;
    }

    return 0.0f;
}

float Joystick<Console::HAC>::GetAxis(int index)
{
    if (!this->IsConnected() || index < 0 || index >= this->GetAxisCount())
        return 0.0f;

    // Buttons and sticks need separate code for each
    if (index < 6)
    {
        switch (this->GetGamepadType())
        {
            case guid::GAMEPAD_TYPE_JOYCON_LEFT:
            {
                if (index / 4 == 0) // No left stick, yes right stick
                    return (index / 2) ? 0 : getStickPosition(this->state, index % 2, false);
                else
                    return getTrigger(padGetButtons(&this->state),
                                      (index % 2) ? HidNpadButton_LeftSR : HidNpadButton_LeftSL);

                break;
            }
            case guid::GAMEPAD_TYPE_JOYCON_RIGHT:
            {
                if (index / 4 == 0) // No left stick, yes right stick
                    return (index / 2) ? 0 : getStickPosition(this->state, index % 2, true);
                else
                    return getTrigger(padGetButtons(&this->state),
                                      (index % 2) ? HidNpadButton_RightSR : HidNpadButton_RightSL);
                break;
            }
            default:
            {
                if (index / 4 == 0)
                    return getStickPosition(this->state, index % 2, (index / 2) == 0);
                else
                    return getTrigger(padGetButtons(&this->state),
                                      (index % 2) ? HidNpadButton_ZR : HidNpadButton_ZL);

                break;
            }
        }
    }

    const auto angle = [&](const auto& haptic) {
        bool isSecondary = haptic.size() == 2 && index >= 12;
        switch (index % 3)
        {
            case 0:
                return getHapticAngle(haptic, isSecondary, ::SixAxis::SIXAXIS_X);
            case 1:
                return getHapticAngle(haptic, isSecondary, ::SixAxis::SIXAXIS_Y);
            case 2:
                return getHapticAngle(haptic, isSecondary, ::SixAxis::SIXAXIS_Z);

            // unreachable
            default:
                return 0.0f;
        }
    };

    switch ((index / 3) % 2)
    {
        case 0:
            return angle(this->sixAxis.GetStateInfo(::SixAxis::SIXAXIS_ACCELEROMETER));
        case 1:
            return angle(this->sixAxis.GetStateInfo(::SixAxis::SIXAXIS_GYROSCOPE));

        // unreachable
        default:
            return 0;
    }

    return 0.0f;
}

std::vector<float> Joystick<Console::HAC>::GetAxes()
{
    std::vector<float> axes {};
    int count = this->GetAxisCount();

    if (!this->IsConnected() || count <= 0)
        return axes;

    axes.reserve(count);

    for (int index = 0; index < count; index++)
        axes.push_back(this->GetAxis(index));

    return axes;
}

bool Joystick<Console::HAC>::IsDown(const std::vector<int>& buttons) const
{
    if (!this->IsConnected())
        return false;

    int count    = this->GetButtonCount();
    auto records = ::buttons.GetEntries().first;

    for (int button : buttons)
    {
        if (button < 0 || button >= count)
            continue;

        if (records[button].second == -1)
            continue;

        if (padGetButtons(&this->state) && records[button].second)
            return true;
    }

    return false;
}

float Joystick<Console::HAC>::GetGamepadAxis(GamepadAxis axis)
{
    if (!this->IsConnected())
        return 0.0f;

    return this->GetAxis(axis);
}

bool Joystick<Console::HAC>::IsGamepadDown(const std::vector<GamepadButton>& buttons) const
{
    HidNpadButton gamepadButton;
    const char* name = nullptr;

    for (auto button : buttons)
    {
        getConstant(button, gamepadButton);

        if (gamepadButton == -1)
            continue;

        if (padGetButtons(&this->state) & (uint32_t)gamepadButton)
            return true;
    }

    return false;
}

void Joystick<Console::HAC>::SetPlayerIndex(int index)
{
    if (!this->IsConnected())
        return;

    if (index < 0 || index > npad::MAX_JOYSTICKS)
        return;

    if (R_SUCCEEDED(hidSwapNpadAssignment(this->playerId, (HidNpadIdType)index)))
        this->playerId = (HidNpadIdType)index;
}

bool Joystick<Console::HAC>::SetVibration(float left, float right, float duration)
{
    left  = std::clamp(left, 0.0f, 1.0f);
    right = std::clamp(right, 0.0f, 1.0f);

    uint32_t length = Vibration<>::MAX;

    if (left == 0.0f && right == 0.0f)
        return this->SetVibration();

    if (!this->IsConnected())
    {
        this->SetVibration();
        return false;
    }

    if (duration >= 0.0f)
        length = std::min(duration, Vibration<>::MAX / 1000.0f);

    if (length == Vibration<>::HAPTYIC_INFINITY)
        this->vibration.SetDuration(length);
    else
        this->vibration.SetDuration(Timer<Console::HAC>::GetTime() + length);

    bool success = this->vibration.SendValues(left, right);

    if (success)
        Module()->AddVibration(&this->vibration);

    return success;
}

bool Joystick<Console::HAC>::SetVibration()
{
    return this->vibration.SendValues(0.0f, 0.0f);
}

void Joystick<Console::HAC>::GetVibration(float& left, float& right)
{
    this->vibration.GetValues(left, right);
}
