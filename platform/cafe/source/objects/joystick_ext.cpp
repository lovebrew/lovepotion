#include <objects/joystick_ext.hpp>
#include <utilities/bidirectionalmap.hpp>

#include <utilities/result.hpp>
#include <utilities/wpad.hpp>

#include <padscore/wpad.h>

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
static constexpr auto nunchuckAxes = BidirectionalMap<>::Create(
    Joystick<>::GAMEPAD_AXIS_LEFTX,        (WPADNunchukButton)(WPAD_NUNCHUK_STICK_EMULATION_LEFT | WPAD_NUNCHUK_STICK_EMULATION_RIGHT),
    Joystick<>::GAMEPAD_AXIS_LEFTY,        (WPADNunchukButton)(WPAD_NUNCHUK_STICK_EMULATION_UP   | WPAD_NUNCHUK_STICK_EMULATION_DOWN),
    Joystick<>::GAMEPAD_AXIS_RIGHTX,       (WPADNunchukButton)-1,
    Joystick<>::GAMEPAD_AXIS_RIGHTY,       (WPADNunchukButton)-1,
    Joystick<>::GAMEPAD_AXIS_TRIGGERLEFT,  (WPADNunchukButton)WPAD_NUNCHUK_BUTTON_Z,
    Joystick<>::GAMEPAD_AXIS_TRIGGERRIGHT, (WPADNunchukButton)-1
);
// clang-format on
/* NUNCHUCK SECTION */
// -----
/* CLASSIC CONTROLLER SECTION */
// clang-format off
constexpr auto wpadClassicButtons = BidirectionalMap<>::Create(
    Joystick<>::GAMEPAD_BUTTON_A,             WPAD_CLASSIC_BUTTON_A,
    Joystick<>::GAMEPAD_BUTTON_B,             WPAD_CLASSIC_BUTTON_B,
    Joystick<>::GAMEPAD_BUTTON_X,             WPAD_CLASSIC_BUTTON_X,
    Joystick<>::GAMEPAD_BUTTON_Y,             WPAD_CLASSIC_BUTTON_Y,
    Joystick<>::GAMEPAD_BUTTON_BACK,          WPAD_CLASSIC_BUTTON_MINUS,
    Joystick<>::GAMEPAD_BUTTON_GUIDE,         WPAD_CLASSIC_BUTTON_HOME,
    Joystick<>::GAMEPAD_BUTTON_START,         WPAD_CLASSIC_BUTTON_PLUS,
    Joystick<>::GAMEPAD_BUTTON_LEFTSHOULDER,  WPAD_CLASSIC_BUTTON_L,
    Joystick<>::GAMEPAD_BUTTON_RIGHTSHOULDER, WPAD_CLASSIC_BUTTON_R,
    Joystick<>::GAMEPAD_BUTTON_LEFTSTICK,     (WPADClassicButton)-1,
    Joystick<>::GAMEPAD_BUTTON_RIGHTSTICK,    (WPADClassicButton)-1,
    Joystick<>::GAMEPAD_BUTTON_DPAD_UP,       WPAD_CLASSIC_BUTTON_UP,
    Joystick<>::GAMEPAD_BUTTON_DPAD_DOWN,     WPAD_CLASSIC_BUTTON_DOWN,
    Joystick<>::GAMEPAD_BUTTON_DPAD_RIGHT,    WPAD_CLASSIC_BUTTON_RIGHT,
    Joystick<>::GAMEPAD_BUTTON_DPAD_LEFT,     WPAD_CLASSIC_BUTTON_LEFT
);

static constexpr auto wpadClassicAxes = BidirectionalMap<>::Create(
    Joystick<>::GAMEPAD_AXIS_LEFTX,        (int32_t)(WPAD_CLASSIC_STICK_L_EMULATION_LEFT | WPAD_CLASSIC_STICK_L_EMULATION_RIGHT),
    Joystick<>::GAMEPAD_AXIS_LEFTY,        (int32_t)(WPAD_CLASSIC_STICK_L_EMULATION_UP   | WPAD_CLASSIC_STICK_L_EMULATION_DOWN),
    Joystick<>::GAMEPAD_AXIS_RIGHTX,       (int32_t)(WPAD_CLASSIC_STICK_R_EMULATION_LEFT | WPAD_CLASSIC_STICK_R_EMULATION_RIGHT),
    Joystick<>::GAMEPAD_AXIS_RIGHTY,       (int32_t)(WPAD_CLASSIC_STICK_R_EMULATION_UP   | WPAD_CLASSIC_STICK_R_EMULATION_DOWN),
    Joystick<>::GAMEPAD_AXIS_TRIGGERLEFT,  (int32_t)WPAD_CLASSIC_BUTTON_ZL,
    Joystick<>::GAMEPAD_AXIS_TRIGGERRIGHT, (int32_t)WPAD_CLASSIC_BUTTON_ZR
);
// clang-format on
/* CLASSIC CONTROLLER SECTION */
//-----
/* PRO CONTROLLER SECTION */
// clang-format off
constexpr auto wpadProButtons = BidirectionalMap<>::Create(
    Joystick<>::GAMEPAD_BUTTON_A,             WPAD_PRO_BUTTON_A,
    Joystick<>::GAMEPAD_BUTTON_B,             WPAD_PRO_BUTTON_B,
    Joystick<>::GAMEPAD_BUTTON_X,             WPAD_PRO_BUTTON_X,
    Joystick<>::GAMEPAD_BUTTON_Y,             WPAD_PRO_BUTTON_Y,
    Joystick<>::GAMEPAD_BUTTON_BACK,          WPAD_PRO_BUTTON_MINUS,
    Joystick<>::GAMEPAD_BUTTON_GUIDE,         WPAD_PRO_BUTTON_HOME,
    Joystick<>::GAMEPAD_BUTTON_START,         WPAD_PRO_BUTTON_PLUS,
    Joystick<>::GAMEPAD_BUTTON_LEFTSHOULDER,  WPAD_PRO_TRIGGER_L,
    Joystick<>::GAMEPAD_BUTTON_RIGHTSHOULDER, WPAD_PRO_TRIGGER_R,
    Joystick<>::GAMEPAD_BUTTON_LEFTSTICK,     WPAD_PRO_BUTTON_STICK_L,
    Joystick<>::GAMEPAD_BUTTON_RIGHTSTICK,    WPAD_PRO_BUTTON_STICK_R,
    Joystick<>::GAMEPAD_BUTTON_DPAD_UP,       WPAD_PRO_BUTTON_UP,
    Joystick<>::GAMEPAD_BUTTON_DPAD_DOWN,     WPAD_PRO_BUTTON_DOWN,
    Joystick<>::GAMEPAD_BUTTON_DPAD_RIGHT,    WPAD_PRO_BUTTON_RIGHT,
    Joystick<>::GAMEPAD_BUTTON_DPAD_LEFT,     WPAD_PRO_BUTTON_LEFT
);


static constexpr auto wpadProAxes = BidirectionalMap<>::Create(
    Joystick<>::GAMEPAD_AXIS_LEFTX,        (int32_t)(WPAD_PRO_STICK_L_EMULATION_LEFT | WPAD_PRO_STICK_L_EMULATION_RIGHT),
    Joystick<>::GAMEPAD_AXIS_LEFTY,        (int32_t)(WPAD_PRO_STICK_L_EMULATION_UP   | WPAD_PRO_STICK_L_EMULATION_DOWN),
    Joystick<>::GAMEPAD_AXIS_RIGHTX,       (int32_t)(WPAD_PRO_STICK_R_EMULATION_LEFT | WPAD_PRO_STICK_R_EMULATION_RIGHT),
    Joystick<>::GAMEPAD_AXIS_RIGHTY,       (int32_t)(WPAD_PRO_STICK_R_EMULATION_UP   | WPAD_PRO_STICK_R_EMULATION_DOWN),
    Joystick<>::GAMEPAD_AXIS_TRIGGERLEFT,  (int32_t)WPAD_PRO_TRIGGER_ZL,
    Joystick<>::GAMEPAD_AXIS_TRIGGERRIGHT, (int32_t)WPAD_PRO_TRIGGER_ZR
);
// clang-format on
/* PRO CONTROLLER SECTION */

Joystick<Console::CAFE>::Joystick(int id) : vpad {}, kpad {}, isGamepad(false), buttonStates {}
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

    this->isGamepad = (index == 0);

    this->leftTrigger  = VPAD_BUTTON_ZL;
    this->rightTrigger = VPAD_BUTTON_ZR;

    if (!this->isGamepad)
    {
        KPADError error {};
        KPADReadEx((KPADChan)index, &this->kpad, 1, &error);

        if (error != KPAD_ERROR_OK || this->kpad.extensionType == 0xFF)
            return false;

        this->extension = (WPADExtensionType)this->kpad.extensionType;

        this->leftTrigger  = -1;
        this->rightTrigger = -1;

        if (this->GetGamepadType() == guid::GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK)
            this->leftTrigger = WPAD_NUNCHUK_BUTTON_Z;
        else if (this->GetGamepadType() == guid::GAMEPAD_TYPE_WII_CLASSIC)
        {
            this->leftTrigger  = WPAD_CLASSIC_BUTTON_ZL;
            this->rightTrigger = WPAD_CLASSIC_BUTTON_ZR;
        }
        else if (this->GetGamepadType() == guid::GAMEPAD_TYPE_WII_PRO)
        {
            this->leftTrigger  = WPAD_PRO_TRIGGER_ZL;
            this->rightTrigger = WPAD_PRO_TRIGGER_ZR;
        }
    }

    this->guid = guid::GetGamepadGUID(this->GetGamepadType());
    this->name = guid::GetGamepadGUID(this->GetGamepadType());

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
    if (!this->IsConnected())
        return 0;

    return guid::GetGamepadAxisCount(this->GetGamepadType());
}

int Joystick<Console::CAFE>::GetButtonCount() const
{
    if (!this->IsConnected())
        return 0;

    return guid::GetGamepadButtonCount(this->GetGamepadType());
}

void Joystick<Console::CAFE>::Update()
{
    if (this->isGamepad)
    {
        VPADReadError error {};
        VPADRead(VPAD_CHAN_0, &this->vpad, 1, &error);

        if (error == VPAD_READ_NO_SAMPLES)
            return;

        this->buttonStates.pressed  = this->vpad.trigger;
        this->buttonStates.released = this->vpad.release;

        this->buttonStates.leftStick  = { this->vpad.leftStick.x, this->vpad.leftStick.y };
        this->buttonStates.rightStick = { this->vpad.rightStick.x, this->vpad.rightStick.y };
    }
    else
    {
        KPADError error {};
        KPADReadEx((WPADChan)(this->playerId - 1), &this->kpad, 1, &error);

        if (error == KPAD_ERROR_NO_SAMPLES)
            return;

        this->buttonStates.pressed  = this->kpad.trigger;
        this->buttonStates.released = this->kpad.release;

        switch (this->GetGamepadType())
        {
            case guid::GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK:
            {
                this->buttonStates.extension.pressed  = this->kpad.nunchuck.trigger;
                this->buttonStates.extension.released = this->kpad.nunchuck.trigger;

                this->buttonStates.leftStick = { this->kpad.nunchuck.stick.x,
                                                 this->kpad.nunchuck.stick.y };

                break;
            }
            case guid::GAMEPAD_TYPE_WII_CLASSIC:
            {
                this->buttonStates.extension.pressed  = this->kpad.classic.trigger;
                this->buttonStates.extension.released = this->kpad.classic.release;

                this->buttonStates.leftStick = { this->kpad.classic.leftStick.x,
                                                 this->kpad.classic.leftStick.y };

                this->buttonStates.rightStick = { this->kpad.classic.rightStick.x,
                                                  this->kpad.classic.rightStick.y };

                break;
            }
            case guid::GAMEPAD_TYPE_WII_PRO:
            {
                this->buttonStates.extension.pressed  = this->kpad.pro.trigger;
                this->buttonStates.extension.released = this->kpad.pro.release;

                this->buttonStates.leftStick = { this->kpad.pro.leftStick.x,
                                                 this->kpad.pro.leftStick.y };

                this->buttonStates.rightStick = { this->kpad.pro.rightStick.x,
                                                  this->kpad.pro.rightStick.y };

                break;
            }
            default:
                break;
        }
    }
}

VPADTouchData Joystick<Console::CAFE>::GetTouchData() const
{
    if (this->isGamepad)
        return this->vpad.tpNormal;

    return VPADTouchData { .x = 0, .y = 0, .touched = 0, .validity = 0 };
}

static bool IsChangedInternal(const auto& buttons, int32_t& buttonState,
                              Joystick<>::JoystickInput& result)
{
    const auto& entries = buttons.GetEntries();

    int32_t button = -1;

    for (size_t index = 0; index < entries.second; index++)
    {
        button = (int32_t)entries.first[index].second;

        if (button == (int32_t)-1)
            continue;

        if (button & buttonState)
        {
            buttonState ^= button;
            result = { .type         = Joystick<>::InputType::INPUT_TYPE_BUTTON,
                       .button       = entries.first[index].first,
                       .buttonNumber = (int)index };

            return true;
        }
    }

    return false;
}

bool Joystick<Console::CAFE>::IsDown(JoystickInput& result)
{
    if (!this->IsConnected())
        return false;

    if (!this->buttonStates.pressed && !this->buttonStates.extension.pressed)
        return false;

    switch (this->GetGamepadType())
    {
        case guid::GAMEPAD_TYPE_WII_U_GAMEPAD:
            return IsChangedInternal(vpadButtons, this->buttonStates.pressed, result);
        case guid::GAMEPAD_TYPE_WII_REMOTE:
            return IsChangedInternal(wpadButtons, this->buttonStates.pressed, result);
        case guid::GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK:
            return IsChangedInternal(wpadButtons, this->buttonStates.pressed, result);
        case guid::GAMEPAD_TYPE_WII_CLASSIC:
            return IsChangedInternal(wpadClassicButtons, this->buttonStates.extension.pressed,
                                     result);
        case guid::GAMEPAD_TYPE_WII_PRO:
            return IsChangedInternal(wpadProButtons, this->buttonStates.extension.pressed, result);
        default:
            break;
    }

    return false;
}

bool Joystick<Console::CAFE>::IsUp(JoystickInput& result)
{
    if (!this->IsConnected())
        return false;

    if (!this->buttonStates.released && !this->buttonStates.extension.released)
        return false;

    switch (this->GetGamepadType())
    {
        case guid::GAMEPAD_TYPE_WII_U_GAMEPAD:
            return IsChangedInternal(vpadButtons, this->buttonStates.released, result);
        case guid::GAMEPAD_TYPE_WII_REMOTE:
            return IsChangedInternal(wpadButtons, this->buttonStates.released, result);
        case guid::GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK:
            return IsChangedInternal(wpadButtons, this->buttonStates.released, result);
        case guid::GAMEPAD_TYPE_WII_CLASSIC:
            return IsChangedInternal(wpadClassicButtons, this->buttonStates.extension.released,
                                     result);
        case guid::GAMEPAD_TYPE_WII_PRO:
            return IsChangedInternal(wpadProButtons, this->buttonStates.extension.released, result);
        default:
            break;
    }

    return false;
}

float Joystick<Console::CAFE>::GetAxis(int index) const
{
    if (!this->IsConnected() || index < 0 || index >= this->GetAxisCount())
        return 0.0f;

    if (index == 0 || index == 1)
    {
        auto stickState = this->buttonStates.leftStick;

        float value = (index == 1) ? stickState.x : stickState.y;
        return value;
    }
    else if (index == 2 || index == 3)
    {
        auto stickState = this->buttonStates.rightStick;

        float value = (index == 1) ? stickState.x : stickState.y;
        return value;
    }
    else if (index == 4)
    {
        if (this->buttonStates.pressed & this->leftTrigger)
            return 1.0f;

        return 0.0f;
    }
    else if (index == 5)
    {
        if (this->buttonStates.pressed & this->rightTrigger)
            return 1.0f;

        return 0.0f;
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
