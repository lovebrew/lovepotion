#include <objects/joystick_ext.hpp>
#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <modules/joystickmodule_ext.hpp>
#include <modules/timer_ext.hpp>

#include <utilities/result.hpp>
#include <utilities/wpad.hpp>

#include <padscore/wpad.h>

using namespace love;

#define Module() (Module::GetInstance<JoystickModule<Console::Which>>(Module::M_JOYSTICK))

template<>
Type Joystick<>::type("Joystick", &Object::type);

// clang-format off
/* VPAD SECTION */
constexpr BidirectionalMap vpadButtons = {
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
};
/* VPAD SECTION */
// ----
/* WPAD SECTION */
constexpr BidirectionalMap wpadButtons = {
    Joystick<>::GAMEPAD_BUTTON_A,             WPAD_BUTTON_A,
    Joystick<>::GAMEPAD_BUTTON_B,             WPAD_BUTTON_B,
    Joystick<>::GAMEPAD_BUTTON_BACK,          WPAD_BUTTON_MINUS,
    Joystick<>::GAMEPAD_BUTTON_GUIDE,         WPAD_BUTTON_HOME,
    Joystick<>::GAMEPAD_BUTTON_START,         WPAD_BUTTON_PLUS,
    Joystick<>::GAMEPAD_BUTTON_DPAD_UP,       WPAD_BUTTON_UP,
    Joystick<>::GAMEPAD_BUTTON_DPAD_DOWN,     WPAD_BUTTON_DOWN,
    Joystick<>::GAMEPAD_BUTTON_DPAD_RIGHT,    WPAD_BUTTON_RIGHT,
    Joystick<>::GAMEPAD_BUTTON_DPAD_LEFT,     WPAD_BUTTON_LEFT
};
/* WPAD SECTION */
// -----
/* NUNCHUCK SECTION */
constexpr BidirectionalMap nunchuckAxes = {
    Joystick<>::GAMEPAD_AXIS_LEFTX,        (WPADNunchukButton)(WPAD_NUNCHUK_STICK_EMULATION_LEFT | WPAD_NUNCHUK_STICK_EMULATION_RIGHT),
    Joystick<>::GAMEPAD_AXIS_LEFTY,        (WPADNunchukButton)(WPAD_NUNCHUK_STICK_EMULATION_UP   | WPAD_NUNCHUK_STICK_EMULATION_DOWN),
    Joystick<>::GAMEPAD_AXIS_TRIGGERLEFT,  (WPADNunchukButton)WPAD_NUNCHUK_BUTTON_Z,
};
constexpr BidirectionalMap nunchuckButtons = {
    Joystick<>::GAMEPAD_BUTTON_LEFTSHOULDER, WPAD_NUNCHUK_BUTTON_C
};
/* NUNCHUCK SECTION */
// -----
/* CLASSIC CONTROLLER SECTION */
constexpr BidirectionalMap wpadClassicButtons = {
    Joystick<>::GAMEPAD_BUTTON_A,             WPAD_CLASSIC_BUTTON_A,
    Joystick<>::GAMEPAD_BUTTON_B,             WPAD_CLASSIC_BUTTON_B,
    Joystick<>::GAMEPAD_BUTTON_X,             WPAD_CLASSIC_BUTTON_X,
    Joystick<>::GAMEPAD_BUTTON_Y,             WPAD_CLASSIC_BUTTON_Y,
    Joystick<>::GAMEPAD_BUTTON_BACK,          WPAD_CLASSIC_BUTTON_MINUS,
    Joystick<>::GAMEPAD_BUTTON_GUIDE,         WPAD_CLASSIC_BUTTON_HOME,
    Joystick<>::GAMEPAD_BUTTON_START,         WPAD_CLASSIC_BUTTON_PLUS,
    Joystick<>::GAMEPAD_BUTTON_LEFTSHOULDER,  WPAD_CLASSIC_BUTTON_L,
    Joystick<>::GAMEPAD_BUTTON_RIGHTSHOULDER, WPAD_CLASSIC_BUTTON_R,
    Joystick<>::GAMEPAD_BUTTON_DPAD_UP,       WPAD_CLASSIC_BUTTON_UP,
    Joystick<>::GAMEPAD_BUTTON_DPAD_DOWN,     WPAD_CLASSIC_BUTTON_DOWN,
    Joystick<>::GAMEPAD_BUTTON_DPAD_RIGHT,    WPAD_CLASSIC_BUTTON_RIGHT,
    Joystick<>::GAMEPAD_BUTTON_DPAD_LEFT,     WPAD_CLASSIC_BUTTON_LEFT
};
/* CLASSIC CONTROLLER SECTION */
//-----
/* PRO CONTROLLER SECTION */
constexpr BidirectionalMap wpadProButtons = {
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
};
// clang-format on
/* PRO CONTROLLER SECTION */

Joystick<Console::CAFE>::Joystick(int id) :
    vpad {},
    kpad {},
    isGamepad(false),
    buttonStates {},
    sixAxis {},
    vibration {}
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
        {
            this->leftTrigger = WPAD_NUNCHUK_BUTTON_Z;
            this->sixAxisExt  = ::SixAxis(false);
        }
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

        this->vibration = ::Vibration((KPADChan)index);
    }

    this->guid = guid::GetGamepadGUID(this->GetGamepadType());
    this->name = guid::GetGamepadGUID(this->GetGamepadType());

    this->sixAxis = ::SixAxis(this->isGamepad);

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
        this->buttonStates.held     = this->vpad.hold;

        this->buttonStates.leftStick  = { this->vpad.leftStick.x, this->vpad.leftStick.y };
        this->buttonStates.rightStick = { this->vpad.rightStick.x, this->vpad.rightStick.y };

        this->sixAxis.Update(this->vpad.accelorometer.acc, SixAxis<>::SIXAXIS_ACCELEROMETER);
        this->sixAxis.Update(this->vpad.gyro, SixAxis<>::SIXAXIS_GYROSCOPE);
    }
    else
    {
        KPADError error {};
        KPADReadEx((WPADChan)(this->playerId - 1), &this->kpad, 1, &error);

        if (error == KPAD_ERROR_NO_SAMPLES)
            return;

        this->buttonStates.pressed  = this->kpad.trigger;
        this->buttonStates.released = this->kpad.release;
        this->buttonStates.held     = this->kpad.hold;
        this->sixAxis.Update(this->kpad.acc, SixAxis<>::SIXAXIS_ACCELEROMETER);

        switch (this->GetGamepadType())
        {
            case guid::GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK:
            {
                this->buttonStates.extension.pressed  = this->kpad.nunchuck.trigger;
                this->buttonStates.extension.released = this->kpad.nunchuck.trigger;

                this->buttonStates.leftStick = { this->kpad.nunchuck.stick.x,
                                                 this->kpad.nunchuck.stick.y };

                this->sixAxisExt.Update(this->kpad.nunchuck.acc, SixAxis<>::SIXAXIS_ACCELEROMETER);

                break;
            }
            case guid::GAMEPAD_TYPE_WII_CLASSIC:
            {
                this->buttonStates.extension.pressed  = this->kpad.classic.trigger;
                this->buttonStates.extension.released = this->kpad.classic.release;
                this->buttonStates.extension.held     = this->kpad.classic.hold;

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
                this->buttonStates.extension.held     = this->kpad.pro.hold;

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

    for (size_t index = 0; index < entries.size(); index++)
    {
        button = (int32_t)entries[index].second;

        if (button == (int32_t)-1)
            continue;

        if (button & buttonState)
        {
            buttonState ^= button;
            result = { .type         = Joystick<>::InputType::INPUT_TYPE_BUTTON,
                       .button       = entries[index].first,
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

static float getStickPosition(Joystick<Console::CAFE>::StickAxis axis, bool isHorizontal)
{
    float value = (isHorizontal) ? axis.x : axis.y;
    return value;
}

static float getTrigger(int32_t held, int32_t button)
{
    if (held & button)
        return 1.0f;

    return 0.0f;
}

float Joystick<Console::CAFE>::GetAxis(int index)
{
    if (!this->IsConnected() || index < 0 || index >= this->GetAxisCount())
        return 0.0f;

    const auto type = this->GetGamepadType();

    switch (type)
    {
        case guid::GAMEPAD_TYPE_WII_U_GAMEPAD:
        {
            if (index == 0 || index == 1)
                return getStickPosition(this->buttonStates.leftStick, (index == 0));
            else if (index == 2 || index == 3)
                return getStickPosition(this->buttonStates.rightStick, (index == 2));
            else if (index == 4)
                return getTrigger(this->buttonStates.held, this->leftTrigger);
            else if (index == 5)
                return getTrigger(this->buttonStates.held, this->rightTrigger);
            else
            {
                if (index >= 6 && index < 9)
                {
                    auto accelerometer = this->sixAxis.GetInfo(SixAxis<>::SIXAXIS_ACCELEROMETER);

                    if (index == 6)
                        return accelerometer.x;
                    else if (index == 7)
                        return accelerometer.y;
                    else
                        return accelerometer.z;
                }

                if (index >= 9 && index < 12)
                {
                    auto gyroscope = this->sixAxis.GetInfo(SixAxis<>::SIXAXIS_GYROSCOPE);

                    if (index == 9)
                        return gyroscope.x;
                    else if (index == 10)
                        return gyroscope.y;
                    else
                        return gyroscope.z;
                }
            }

            break;
        }
        case guid::GAMEPAD_TYPE_WII_REMOTE:
        {
            auto accelerometer = this->sixAxis.GetInfo(SixAxis<>::SIXAXIS_ACCELEROMETER);

            if (index >= 0 && index < 3)
            {
                if (index == 0)
                    return accelerometer.x;
                else if (index == 1)
                    return accelerometer.y;
                else
                    return accelerometer.z;
            }

            break;
        }
        case guid::GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK:
        {
            if (index == 0 || index == 1)
                return getStickPosition(this->buttonStates.leftStick, (index == 0));
            else if (index == 2)
                return getTrigger(this->buttonStates.extension.held, this->leftTrigger);
            else
            {
                if (index >= 3 && index < 6)
                {
                    auto acceleromter = this->sixAxis.GetInfo(SixAxis<>::SIXAXIS_ACCELEROMETER);

                    if (index == 3)
                        return acceleromter.x;
                    else if (index == 4)
                        return acceleromter.y;
                    else
                        return acceleromter.z;
                }

                if (index >= 6 && index < 9)
                {
                    auto acceleromter = this->sixAxisExt.GetInfo(SixAxis<>::SIXAXIS_ACCELEROMETER);

                    if (index == 6)
                        return acceleromter.x;
                    else if (index == 7)
                        return acceleromter.y;
                    else
                        return acceleromter.z;
                }
            }
            break;
        }
        case guid::GAMEPAD_TYPE_WII_CLASSIC:
        case guid::GAMEPAD_TYPE_WII_PRO:
        {
            if (index == 0 || index == 1)
                return getStickPosition(this->buttonStates.leftStick, (index == 0));
            else if (index == 2 || index == 3)
                return getStickPosition(this->buttonStates.rightStick, (index == 2));
            else if (index == 4)
                return getTrigger(this->buttonStates.held, this->leftTrigger);
            else if (index == 5)
                return getTrigger(this->buttonStates.held, this->rightTrigger);

            break;
        }
        default:
            break;
    }

    return 0.0f;
}

std::vector<float> Joystick<Console::CAFE>::GetAxes()
{
    std::vector<float> axes {};
    int count = this->GetAxisCount();

    if (!this->IsConnected() || count <= 0)
        return axes;

    for (int index = 0; index < count; index++)
        axes.push_back(this->GetAxis(index));

    return axes;
}

static bool isDownInternal(const auto& buttons, uint32_t button, uint32_t held)
{
    const auto& records = buttons.GetEntries();

    return false;
}

bool Joystick<Console::CAFE>::IsDown(const std::vector<int>& buttons) const
{
    if (!this->IsConnected())
        return false;

    int count = this->GetButtonCount();

    for (auto& button : buttons)
    {
        if (button < 0 || button >= count)
            continue;

        switch (this->GetGamepadType())
        {
            case guid::GAMEPAD_TYPE_WII_U_GAMEPAD:
            {
                const auto& entries = vpadButtons.GetEntries();

                if (entries[button].second == (VPADButtons)-1)
                    continue;

                if (this->buttonStates.held & entries[button].second)
                    return true;

                break;
            }
            case guid::GAMEPAD_TYPE_WII_REMOTE:
            {
                const auto& entries = wpadButtons.GetEntries();

                if (entries[button].second == (WPADButton)-1)
                    continue;

                if (this->buttonStates.held & entries[button].second)
                    return true;

                break;
            }
            case guid::GAMEPAD_TYPE_WII_CLASSIC:
            {
                const auto& entries = wpadClassicButtons.GetEntries();

                if (entries[button].second == (WPADClassicButton)-1)
                    continue;

                if (this->buttonStates.held & entries[button].second)
                    return true;

                break;
            }
            case guid::GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK:
            {
                const auto& entries = wpadButtons.GetEntries();

                if (entries[button].second != (WPADButton)-1)
                {
                    if (this->buttonStates.held & entries[button].second)
                        return true;
                }

                if (button == 10)
                {
                    if (this->buttonStates.extension.held & WPAD_NUNCHUK_BUTTON_C)
                        return true;
                }

                break;
            }
            case guid::GAMEPAD_TYPE_WII_PRO:
            {
                const auto& entries = wpadProButtons.GetEntries();

                if (entries[button].second == (WPADProButton)-1)
                    continue;

                if (this->buttonStates.held & entries[button].second)
                    return true;

                break;
            }
            default:
                break;
        }
    }

    return false;
}

float Joystick<Console::CAFE>::GetGamepadAxis(GamepadAxis axis)
{
    if (!this->IsConnected())
        return 0.0f;

    return this->GetAxis(axis);
}

bool Joystick<Console::CAFE>::IsGamepadDown(const std::vector<GamepadButton>& buttons) const
{
    const auto type = this->GetGamepadType();

    for (auto button : buttons)
    {
        switch (type)
        {
            case guid::GAMEPAD_TYPE_WII_U_GAMEPAD:
            {
                if (auto gamepadButton = vpadButtons.Find(button))
                    return this->buttonStates.held & *gamepadButton;

                break;
            }
            case guid::GAMEPAD_TYPE_WII_REMOTE:
            {
                if (auto gamepadButton = wpadButtons.Find(button))
                    return this->buttonStates.held & *gamepadButton;

                break;
            }
            case guid::GAMEPAD_TYPE_WII_CLASSIC:
            {
                if (auto gamepadButton = wpadClassicButtons.Find(button))
                    return this->buttonStates.held & *gamepadButton;

                break;
            }
            case guid::GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK:
            {
                if (auto gamepadButton = wpadButtons.Find(button))
                    return this->buttonStates.held & *gamepadButton;
                else if (auto extButton = nunchuckButtons.Find(button))
                    return this->buttonStates.held & *extButton;

                break;
            }
            case guid::GAMEPAD_TYPE_WII_PRO:
            {
                if (auto gamepadButton = wpadProButtons.Find(button))
                    return this->buttonStates.held & *gamepadButton;

                break;
            }
            default:
                break;
        };
    }

    return false;
}

void Joystick<Console::CAFE>::SetPlayerIndex(int index)
{
    if (!this->IsConnected())
        return;
}

bool Joystick<Console::CAFE>::SetVibration(float left, float right, float duration)
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
        this->vibration.SetDuration(Timer<Console::CAFE>::GetTime() + length);

    bool success = this->vibration.SendValues(left, right);

    if (success)
        Module()->AddVibration(&this->vibration);

    return success;
}

bool Joystick<Console::CAFE>::SetVibration()
{
    return this->vibration.Stop();
}

void Joystick<Console::CAFE>::GetVibration(float& left, float& right)
{
    this->vibration.GetValues(left, right);
}
