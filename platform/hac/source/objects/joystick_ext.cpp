#include <objects/joystick_ext.hpp>
#include <utilities/npad.hpp>

#include <utilities/bidirectionalmap.hpp>

using namespace love;

template<>
Type Joystick<>::type("Joystick", &Object::type);

// clang-format off
constexpr auto buttons = BidirectionalMap<>::Create(
    Joystick<>::GAMEPAD_BUTTON_A,             HidNpadButton_A,
    Joystick<>::GAMEPAD_BUTTON_B,             HidNpadButton_B,
    Joystick<>::GAMEPAD_BUTTON_X,             HidNpadButton_X,
    Joystick<>::GAMEPAD_BUTTON_Y,             HidNpadButton_Y,
    Joystick<>::GAMEPAD_BUTTON_BACK,          HidNpadButton_Minus,
    Joystick<>::GAMEPAD_BUTTON_GUIDE,         (int)-1,
    Joystick<>::GAMEPAD_BUTTON_START,         HidNpadButton_Plus,
    Joystick<>::GAMEPAD_BUTTON_LEFTSHOULDER,  HidNpadButton_L,
    Joystick<>::GAMEPAD_BUTTON_RIGHTSHOULDER, HidNpadButton_R,
    Joystick<>::GAMEPAD_BUTTON_LEFTSTICK,     HidNpadButton_StickL,
    Joystick<>::GAMEPAD_BUTTON_RIGHTSTICK,    HidNpadButton_StickR,
    Joystick<>::GAMEPAD_BUTTON_DPAD_UP,       HidNpadButton_Up,
    Joystick<>::GAMEPAD_BUTTON_DPAD_DOWN,     HidNpadButton_Down,
    Joystick<>::GAMEPAD_BUTTON_DPAD_RIGHT,    HidNpadButton_Right,
    Joystick<>::GAMEPAD_BUTTON_DPAD_LEFT,     HidNpadButton_Left
);
// clang-format on

bool Joystick<Console::HAC>::GetConstant(GamepadButton in, int& out)
{
    return buttons.Find(in, out);
}

bool Joystick<Console::HAC>::GetConstant(int in, GamepadButton& out)
{
    return buttons.ReverseFind(in, out);
}

// clang-format off
static constexpr auto axes = BidirectionalMap<>::Create(
    Joystick<>::GAMEPAD_AXIS_LEFTX,        (uint64_t)(HidNpadButton_StickLLeft | HidNpadButton_StickLRight),
    Joystick<>::GAMEPAD_AXIS_LEFTY,        (uint64_t)(HidNpadButton_StickLUp   | HidNpadButton_StickLDown),
    Joystick<>::GAMEPAD_AXIS_RIGHTX,       (uint64_t)(HidNpadButton_StickRLeft | HidNpadButton_StickRRight),
    Joystick<>::GAMEPAD_AXIS_RIGHTY,       (uint64_t)(HidNpadButton_StickRUp   | HidNpadButton_StickRDown),
    Joystick<>::GAMEPAD_AXIS_TRIGGERLEFT,  (uint64_t)(HidNpadButton_ZL),
    Joystick<>::GAMEPAD_AXIS_TRIGGERRIGHT, (uint64_t)(HidNpadButton_ZR)
);
// clang-format on

bool Joystick<Console::HAC>::GetConstant(GamepadAxis in, uint64_t& out)
{
    return axes.Find(in, out);
}

bool Joystick<Console::HAC>::GetConstant(uint64_t in, GamepadAxis& out)
{
    return axes.ReverseFind(in, out);
}

Joystick<Console::HAC>::Joystick(int id) : buttonStates {}
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
    this->playerId   = (HidNpadIdType)index;

    this->guid = guid::GetDeviceGUID(this->GetGamepadType());

    if (this->style == (HidNpadStyleTag)-1)
        return false;

    this->name = guid::GetDeviceName(this->GetGamepadType());

    /* todo: sixaxis and vibration stuff */

    return this->IsConnected();
}

void Joystick<Console::HAC>::Close()
{
    this->instanceId = -1;
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
    return this->IsConnected() ? 12 : 0;
}

int Joystick<Console::HAC>::GetButtonCount() const
{
    /* todo: check horizontal single joycons */
    if (this->style == HidNpadStyleTag_NpadJoyLeft || this->style == HidNpadStyleTag_NpadJoyRight)
        return this->IsConnected() ? 6 : 0;

    return this->IsConnected() ? 14 : 0;
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

float Joystick<Console::HAC>::GetAxis(int index) const
{
    if (!this->IsConnected() || index < 0 || index >= this->GetAxisCount())
        return 0.0f;

    if (index == 1 || index == 2)
    {
        auto stickState = padGetStickPos(&this->state, 0);

        float value = (index == 1) ? stickState.x : stickState.y;
        return std::clamp<float>(value / Joystick::JoystickMax, 0, 1.0f);
    }
    else if (index == 3 || index == 4)
    {
        auto stickState = padGetStickPos(&this->state, 1);

        float value = (index == 3) ? stickState.x : stickState.y;
        return std::clamp<float>(value / Joystick::JoystickMax, 0, 1.0f);
    }
    else if (index == 5)
    {
        if (padGetButtons(&this->state) & HidNpadButton_ZL)
            return 1.0f;

        return 0.0f;
    }
    else if (index == 6)
    {
        if (padGetButtons(&this->state) & HidNpadButton_ZR)
            return 1.0f;

        return 0.0f;
    }
    else
    {
        /* todo handle sixaxis */
    }

    return 0.0f;
}

std::vector<float> Joystick<Console::HAC>::GetAxes() const
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

float Joystick<Console::HAC>::GetGamepadAxis(GamepadAxis axis) const
{
    if (!this->IsConnected())
        return 0.0f;

    return this->GetAxis((int)axis);
}

bool Joystick<Console::HAC>::IsGamepadDown(const std::vector<GamepadButton>& buttons) const
{
    GamepadButton gamepadButton;
    const char* name = nullptr;

    for (auto button : buttons)
    {
        if (!Joystick<>::GetConstant(button, name))
            continue;

        if (!Joystick<>::GetConstant(name, gamepadButton))
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

/* todo VIBRATE */
bool Joystick<Console::HAC>::SetVibration(float left, float right, float duration)
{
    return false;
}

/* todo STOP VIBRATING */
bool Joystick<Console::HAC>::SetVibration()
{
    return false;
}

/* todo GET VIBRATION */
void Joystick<Console::HAC>::GetVibration(float& left, float& right)
{}

// clang-format off
constexpr auto deviceTypes = BidirectionalMap<>::Create(
    "left",  HidNpadJoyDeviceType_Left,
    "right", HidNpadJoyDeviceType_Right
);
// clang-format off

bool Joystick<Console::HAC>::GetConstant(const char*in, HidNpadJoyDeviceType&out)
{
    return deviceTypes.Find(in, out);
}

bool Joystick<Console::HAC>::GetConstant(HidNpadJoyDeviceType in, const char*& out)
{
    return deviceTypes.ReverseFind(in, out);
}

std::vector<const char*> Joystick<Console::HAC>::GetConstants(HidNpadJoyDeviceType)
{
    return deviceTypes.GetNames();
}
