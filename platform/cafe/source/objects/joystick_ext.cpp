#include <objects/joystick_ext.hpp>
#include <utilities/bidirectionalmap.hpp>

#include <utilities/result.hpp>
#include <utilities/wpad.hpp>

#include <utilities/log/logfile.h>

using namespace love;

template<>
Type Joystick<>::type("Joystick", &Object::type);

// clang-format off
/* VPAD SECTION */
constexpr auto vpadButtons = BidirectionalMap<>::Create(
    Joystick<>::GAMEPAD_BUTTON_A,             VPAD_BUTTON_A,
    Joystick<>::GAMEPAD_BUTTON_B,             VPAD_BUTTON_B,
    Joystick<>::GAMEPAD_BUTTON_X,             VPAD_BUTTON_X,
    Joystick<>::GAMEPAD_BUTTON_Y,             VPAD_BUTTON_Y,
    Joystick<>::GAMEPAD_BUTTON_BACK,          VPAD_BUTTON_MINUS,
    Joystick<>::GAMEPAD_BUTTON_GUIDE,         VPAD_BUTTON_HOME,
    Joystick<>::GAMEPAD_BUTTON_START,         VPAD_BUTTON_PLUS,
    Joystick<>::GAMEPAD_BUTTON_LEFTSHOULDER,  VPAD_BUTTON_L,
    Joystick<>::GAMEPAD_BUTTON_RIGHTSHOULDER, VPAD_BUTTON_R,
    Joystick<>::GAMEPAD_BUTTON_LEFTSTICK,     VPAD_BUTTON_STICK_L,
    Joystick<>::GAMEPAD_BUTTON_RIGHTSTICK,    VPAD_BUTTON_STICK_R,
    Joystick<>::GAMEPAD_BUTTON_DPAD_UP,       VPAD_BUTTON_UP,
    Joystick<>::GAMEPAD_BUTTON_DPAD_DOWN,     VPAD_BUTTON_DOWN,
    Joystick<>::GAMEPAD_BUTTON_DPAD_RIGHT,    VPAD_BUTTON_RIGHT,
    Joystick<>::GAMEPAD_BUTTON_DPAD_LEFT,     VPAD_BUTTON_LEFT
);

constexpr auto vpadAxes = BidirectionalMap<>::Create(
    Joystick<>::GAMEPAD_AXIS_LEFTX,        (int32_t)(VPAD_STICK_L_EMULATION_LEFT | VPAD_STICK_L_EMULATION_RIGHT),
    Joystick<>::GAMEPAD_AXIS_LEFTY,        (int32_t)(VPAD_STICK_L_EMULATION_UP   | VPAD_STICK_L_EMULATION_DOWN),
    Joystick<>::GAMEPAD_AXIS_RIGHTX,       (int32_t)(VPAD_STICK_L_EMULATION_LEFT | VPAD_STICK_R_EMULATION_RIGHT),
    Joystick<>::GAMEPAD_AXIS_RIGHTY,       (int32_t)(VPAD_STICK_R_EMULATION_UP   | VPAD_STICK_R_EMULATION_DOWN),
    Joystick<>::GAMEPAD_AXIS_TRIGGERLEFT,  (int32_t)(VPAD_BUTTON_ZL),
    Joystick<>::GAMEPAD_AXIS_TRIGGERRIGHT, (int32_t)(VPAD_BUTTON_ZR)
);
// clang-format on
bool Joystick<Console::CAFE>::GetConstant(GamepadButton in, VPADButtons& out)
{
    return vpadButtons.Find(in, out);
}

bool Joystick<Console::CAFE>::GetConstant(VPADButtons in, GamepadButton& out)
{
    return vpadButtons.ReverseFind(in, out);
}

bool Joystick<Console::CAFE>::GetConstant(GamepadAxis in, int32_t& out)
{
    return vpadAxes.Find(in, out);
}

bool Joystick<Console::CAFE>::GetConstant(int32_t in, GamepadAxis& out)
{
    return vpadAxes.ReverseFind(in, out);
}
/* VPAD SECTION */
// ----
// clang-format off
/* WPAD SECTION */
constexpr auto wpadButtons = BidirectionalMap<>::Create(
    Joystick<>::GAMEPAD_BUTTON_A,             WPAD_BUTTON_A,
    Joystick<>::GAMEPAD_BUTTON_B,             WPAD_BUTTON_B,
    Joystick<>::GAMEPAD_BUTTON_X,             (WPADButton)-1,
    Joystick<>::GAMEPAD_BUTTON_Y,             (WPADButton)-1,
    Joystick<>::GAMEPAD_BUTTON_BACK,          WPAD_BUTTON_MINUS,
    Joystick<>::GAMEPAD_BUTTON_GUIDE,         WPAD_BUTTON_HOME,
    Joystick<>::GAMEPAD_BUTTON_START,         WPAD_BUTTON_PLUS,
    Joystick<>::GAMEPAD_BUTTON_LEFTSHOULDER,  (WPADButton)-1,
    Joystick<>::GAMEPAD_BUTTON_RIGHTSHOULDER, (WPADButton)-1,
    Joystick<>::GAMEPAD_BUTTON_LEFTSTICK,     (WPADButton)-1,
    Joystick<>::GAMEPAD_BUTTON_RIGHTSTICK,    (WPADButton)-1,
    Joystick<>::GAMEPAD_BUTTON_DPAD_UP,       WPAD_BUTTON_UP,
    Joystick<>::GAMEPAD_BUTTON_DPAD_DOWN,     WPAD_BUTTON_DOWN,
    Joystick<>::GAMEPAD_BUTTON_DPAD_RIGHT,    WPAD_BUTTON_RIGHT,
    Joystick<>::GAMEPAD_BUTTON_DPAD_LEFT,     WPAD_BUTTON_LEFT
);
// clang-format on

bool Joystick<Console::CAFE>::GetConstant(GamepadButton in, WPADButton& out)
{
    return wpadButtons.Find(in, out);
}

bool Joystick<Console::CAFE>::GetConstant(WPADButton in, GamepadButton& out)
{
    return wpadButtons.ReverseFind(in, out);
}
/* WPAD SECTION */
// -----
/* NUNCHUCK SECTION */
// clang-format off
static constexpr auto axes = BidirectionalMap<>::Create(
    Joystick<>::GAMEPAD_AXIS_LEFTX,        (WPADNunchukButton)(WPAD_NUNCHUK_STICK_EMULATION_LEFT | WPAD_NUNCHUK_STICK_EMULATION_RIGHT),
    Joystick<>::GAMEPAD_AXIS_LEFTY,        (WPADNunchukButton)(WPAD_NUNCHUK_STICK_EMULATION_UP   | WPAD_NUNCHUK_STICK_EMULATION_DOWN),
    Joystick<>::GAMEPAD_AXIS_RIGHTX,       (WPADNunchukButton)-1,
    Joystick<>::GAMEPAD_AXIS_RIGHTY,       (WPADNunchukButton)-1,
    Joystick<>::GAMEPAD_AXIS_TRIGGERLEFT,  (WPADNunchukButton)-1,
    Joystick<>::GAMEPAD_AXIS_TRIGGERRIGHT, (WPADNunchukButton)-1
);
// clang-format on
/* NUNCHUCK SECTION */

Joystick<Console::CAFE>::Joystick(int id) : kpad {}, isGamepad(false), buttonStates {}
{
    this->instanceId = -1;
    this->id         = id;
}

Joystick<Console::CAFE>::Joystick(int id, int index) : Joystick(id)
{
    this->Open(index);
}

Joystick<Console::CAFE>::~Joystick()
{
    this->Close();
}

bool Joystick<Console::CAFE>::Open(int index)
{
    this->Close();

    this->instanceId = index;
    this->playerId   = index;

    this->guid = guid::GetDeviceGUID(this->GetGamepadType());
    this->name = guid::GetDeviceName(this->GetGamepadType());

    this->isGamepad = (index == 0);

    if (!this->isGamepad)
        WPADProbe((WPADChan)(index - 1), &this->extension);

    return this->IsConnected();
}

void Joystick<Console::CAFE>::Close()
{
    this->instanceId = -1;
}

guid::GamepadType Joystick<Console::CAFE>::GetGamepadType() const
{
    if (this->isGamepad)
        return guid::GAMEPAD_TYPE_WII_U_GAMEPAD;

    return wpad::GetWPADType(this->extension);
}

void Joystick<Console::CAFE>::GetDeviceInfo(int& vendor, int& product, int& version)
{}

bool Joystick<Console::CAFE>::IsConnected() const
{
    if (this->isGamepad)
        return true;

    WPADExtensionType type;
    const auto success = WPADProbe((WPADChan)(this->playerId - 1), &type);

    return ResultCode(success).Success();
}

int Joystick<Console::CAFE>::GetAxisCount() const
{
    if (this->isGamepad)
        return 12;

    switch (this->GetGamepadType())
    {
        case guid::GAMEPAD_TYPE_WII_REMOTE:
            return 0;
        case guid::GAMEPAD_TYPE_WII_CLASSIC:
        case guid::GAMEPAD_TYPE_WII_PRO:
            return 6;
        case guid::GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK:
            return 2;
        default:
            break;
    }

    return 0;
}

int Joystick<Console::CAFE>::GetButtonCount() const
{
    if (this->isGamepad)
        return 15;

    /* todo: other types */
    switch (this->GetGamepadType())
    {
        default:
            break;
    }

    return 0;
}

void Joystick<Console::CAFE>::Update(const VPADStatus& status)
{
    if (this->isGamepad)
    {
        this->buttonStates.pressed  = status.trigger;
        this->buttonStates.released = status.release;

        this->buttonStates.leftStick  = status.leftStick;
        this->buttonStates.rightStick = status.rightStick;
    }
    else
    {
        KPADRead((WPADChan)(this->playerId - 1), &this->kpad, 1);

        /* todo: handle other types */
        this->buttonStates.pressed  = this->kpad.trigger;
        this->buttonStates.released = this->kpad.release;
    }
}

bool Joystick<Console::CAFE>::IsDown(JoystickInput& result)
{
    if (!this->IsConnected())
        return false;

    if (!this->buttonStates.pressed)
        return false;

    const auto entries = vpadButtons.GetEntries();
    int32_t button     = -1;

    for (size_t index = 0; index < entries.second; index++)
    {
        button = entries.first[index].second;

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

bool Joystick<Console::CAFE>::IsUp(JoystickInput& result)
{
    if (!this->IsConnected())
        return false;

    if (!this->buttonStates.released)
        return false;

    const auto entries = vpadButtons.GetEntries();
    int32_t button     = -1;

    for (size_t index = 0; index < entries.second; index++)
    {
        button = entries.first[index].second;

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

float Joystick<Console::CAFE>::GetAxis(int index) const
{
    if (!this->IsConnected() || index < 0 || index >= this->GetAxisCount())
        return 0.0f;

    if (this->isGamepad)
    {
        if (index == 1 || index == 2)
        {
            auto stickState = this->buttonStates.leftStick;

            float value = (index == 1) ? stickState.x : stickState.y;
            return value;
        }
        else if (index == 3 || index == 4)
        {
            auto stickState = this->buttonStates.rightStick;

            float value = (index == 1) ? stickState.x : stickState.y;
            return value;
        }
        else if (index == 5)
        {
            if (this->buttonStates.pressed & VPAD_BUTTON_ZL)
                return 1.0f;

            return 0.0f;
        }
        else if (index == 6)
        {
            if (this->buttonStates.pressed & VPAD_BUTTON_ZR)
                return 1.0f;

            return 0.0f;
        }
        else /* todo: handle gyro/accel */
        {}
    }
    else
    {
        /* todo: handle others */
    }

    return 0.0f;
}

std::vector<float> Joystick<Console::CAFE>::GetAxes() const
{
    std::vector<float> axes {};
    int count = this->GetAxisCount();

    if (!this->IsConnected() || count <= 0)
        return axes;

    for (int index = 0; index < count; index++)
        axes.push_back(this->GetAxis(index));

    return axes;
}

/* todo */
bool Joystick<Console::CAFE>::IsDown(const std::vector<int>& buttons) const
{
    return false;
}

/* todo */
float Joystick<Console::CAFE>::GetGamepadAxis(GamepadAxis axis) const
{
    if (!this->IsConnected())
        return 0.0f;

    return 0.0f;
}

/* todo */
bool Joystick<Console::CAFE>::IsGamepadDown(const std::vector<GamepadButton>& buttons) const
{
    return false;
}

void Joystick<Console::CAFE>::SetPlayerIndex(int index)
{
    if (!this->IsConnected())
        return;
}

/* todo */
bool Joystick<Console::CAFE>::SetVibration(float left, float right, float duration)
{
    return false;
}

/* todo */
bool Joystick<Console::CAFE>::SetVibration()
{
    return false;
}

/* todo */
void Joystick<Console::CAFE>::GetVibration(float& left, float& right)
{}
