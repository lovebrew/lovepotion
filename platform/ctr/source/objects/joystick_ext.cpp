#include <objects/joystick_ext.hpp>
#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <algorithm>
#include <limits>

using namespace love;

// clang-format off
constexpr BidirectionalMap buttons = {
    Joystick<>::GAMEPAD_BUTTON_A,             KEY_A,
    Joystick<>::GAMEPAD_BUTTON_B,             KEY_B,
    Joystick<>::GAMEPAD_BUTTON_X,             KEY_X,
    Joystick<>::GAMEPAD_BUTTON_Y,             KEY_Y,
    Joystick<>::GAMEPAD_BUTTON_BACK,          KEY_SELECT,
    Joystick<>::GAMEPAD_BUTTON_START,         KEY_START,
    Joystick<>::GAMEPAD_BUTTON_LEFTSHOULDER,  KEY_L,
    Joystick<>::GAMEPAD_BUTTON_RIGHTSHOULDER, KEY_R,
    Joystick<>::GAMEPAD_BUTTON_DPAD_UP,       KEY_DUP,
    Joystick<>::GAMEPAD_BUTTON_DPAD_DOWN,     KEY_DDOWN,
    Joystick<>::GAMEPAD_BUTTON_DPAD_RIGHT,    KEY_DRIGHT,
    Joystick<>::GAMEPAD_BUTTON_DPAD_LEFT,     KEY_DLEFT
};

constexpr BidirectionalMap axes = {
    Joystick<>::GAMEPAD_AXIS_LEFTX,  KEY_CPAD_LEFT   | KEY_CPAD_RIGHT,
    Joystick<>::GAMEPAD_AXIS_LEFTY,  KEY_CPAD_UP     | KEY_CPAD_DOWN,
    Joystick<>::GAMEPAD_AXIS_RIGHTX, KEY_CSTICK_LEFT | KEY_CSTICK_RIGHT,
    Joystick<>::GAMEPAD_AXIS_RIGHTY, KEY_CSTICK_UP   | KEY_CSTICK_DOWN
};
// clang-format on

Joystick<Console::CTR>::Joystick(int id) : buttonStates {}
{
    this->instanceId = -1;
    this->id         = id;

    this->handle = std::make_unique<uint8_t>(1);
}

Joystick<Console::CTR>::Joystick(int id, int index) : Joystick(id)
{
    this->Open(index);
}

Joystick<Console::CTR>::~Joystick()
{
    this->Close();
}

bool Joystick<Console::CTR>::Open(int index)
{
    this->Close();

    this->name = guid::GetGamepadName(this->GetGamepadType());
    this->guid = guid::GetGamepadGUID(this->GetGamepadType());

    this->instanceId = index;

    return this->IsConnected();
}

void Joystick<Console::CTR>::Close()
{
    this->instanceId = -1;
}

void Joystick<Console::CTR>::GetDeviceInfo(int& vendor, int& product, int& version)
{
    guid::DeviceInfo info {};

    if (!guid::GetDeviceInfo(this->GetGamepadType(), info))
        return;

    vendor  = info.vendorId;
    product = info.productId;
    version = info.productVersion;
}

int Joystick<Console::CTR>::GetAxisCount() const
{
    return guid::GetGamepadAxisCount(this->GetGamepadType());
}

int Joystick<Console::CTR>::GetButtonCount() const
{
    return guid::GetGamepadButtonCount(this->GetGamepadType());
}

void Joystick<Console::CTR>::Update()
{
    this->buttonStates.pressed  = hidKeysDown();
    this->buttonStates.released = hidKeysUp();
    this->buttonStates.held     = hidKeysHeld();
}

bool Joystick<Console::CTR>::IsDown(JoystickInput& result)
{
    if (!this->IsConnected())
        return false;

    if (!this->buttonStates.pressed)
        return false;

    uint32_t button = 0;

    const auto entries = buttons.GetEntries();

    for (size_t index = 0; index < entries.size(); index++)
    {
        button = (uint32_t)entries[index].second;

        if ((int)entries[index].second == -1)
            continue;

        if (button & this->buttonStates.pressed)
        {
            this->buttonStates.pressed ^= button;
            result = { .type         = InputType::INPUT_TYPE_BUTTON,
                       .button       = entries[index].first,
                       .buttonNumber = (int)index };

            return true;
        }
    }

    return false;
}

bool Joystick<Console::CTR>::IsUp(JoystickInput& result)
{
    uint32_t button = 0;

    if (!this->buttonStates.released)
        return false;

    const auto entries = buttons.GetEntries();

    for (size_t index = 0; index < entries.size(); index++)
    {
        button = (uint32_t)entries[index].second;

        if ((int)entries[index].second == -1)
            continue;

        if (button & this->buttonStates.released)
        {
            this->buttonStates.released ^= button;
            result = { .type         = InputType::INPUT_TYPE_BUTTON,
                       .button       = entries[index].first,
                       .buttonNumber = (int)index };

            return true;
        }
    }

    return false;
}

float Joystick<Console::CTR>::GetAxis(int index)
{
    if (!this->IsConnected() || index < 0 || index >= this->GetAxisCount())
        return 0.0f;

    if (index == 0 || index == 1)
    {
        circlePosition leftStick {};
        hidCircleRead(&leftStick);

        float value = (index == 1) ? leftStick.dx : leftStick.dy;
        return std::clamp<float>(value / Joystick::JoystickMax, 0, 1.0f);
    }
    else if (index == 2 || index == 3)
    {
        circlePosition rightStick {};
        irrstCstickRead(&rightStick);

        float value = (index == 3) ? rightStick.dx : rightStick.dy;
        return std::clamp<float>(value / Joystick::JoystickMax, 0, 1.0f);
    }
    else if (index == 4)
    {
        if (hidKeysHeld() & KEY_ZL)
            return 1.0f;

        return 0.0f;
    }
    else if (index == 5)
    {
        if (hidKeysHeld() & KEY_ZR)
            return 1.0f;

        return 0.0f;
    }

    return 0.0f;
}

std::vector<float> Joystick<Console::CTR>::GetAxes()
{
    std::vector<float> axes;
    int count = this->GetAxisCount();

    if (!this->IsConnected() || count <= 0)
        return axes;

    axes.reserve(count);

    for (int index = 0; index < count; index++)
        axes.push_back(this->GetAxis(index));

    return axes;
}

bool Joystick<Console::CTR>::IsAxisChanged(GamepadAxis axis)
{
    auto dsAxis = *axes.Find(axis);

    if (dsAxis & this->buttonStates.held)
    {
        this->buttonStates.held ^= dsAxis;
        return true;
    }

    if (dsAxis & this->buttonStates.released)
    {
        this->buttonStates.released ^= dsAxis;
        return true;
    }

    return false;
}

bool Joystick<Console::CTR>::HasSensor(Sensor::SensorType type) const
{
    return true;
}

bool Joystick<Console::CTR>::IsSensorEnabled(Sensor::SensorType type)
{
    return this->sensors[type];
}

void Joystick<Console::CTR>::SetSensorEnabled(Sensor::SensorType type, bool enabled)
{
    if (this->sensors[type] && !enabled)
        this->sensors[type] = nullptr;
    else if (this->sensors[type] == nullptr && enabled)
    {
        SensorBase* sensor = nullptr;

        if (type == Sensor::SENSOR_ACCELEROMETER)
            sensor = new Accelerometer();
        else if (type == Sensor::SENSOR_GYROSCOPE)
            sensor = new Gyroscope();

        sensor->SetEnabled(enabled);
        this->sensors[type] = sensor;
    }
}

std::vector<float> Joystick<Console::CTR>::GetSensorData(Sensor::SensorType type)
{
    if (!this->IsSensorEnabled(type))
    {
        auto name = Sensor::sensorTypes.ReverseFind(type);
        throw love::Exception("\"%s\" sensor is not enabled", *name);
    }

    return this->sensors[type]->GetData();
}

bool Joystick<Console::CTR>::IsDown(const std::vector<int>& buttons) const
{
    if (!this->IsConnected())
        return false;

    int count    = this->GetButtonCount();
    auto records = ::buttons.GetEntries();

    for (int button : buttons)
    {
        if (button < 0 || button >= count)
            continue;

        if (hidKeysHeld() & records[button].second)
            return true;
    }

    return false;
}

float Joystick<Console::CTR>::GetGamepadAxis(GamepadAxis axis)
{
    if (!this->IsConnected())
        return 0.0f;

    int getAxis = (int)axis;
    return this->GetAxis(getAxis - 1);
}

bool Joystick<Console::CTR>::IsGamepadDown(const std::vector<GamepadButton>& buttons) const
{
    uint32_t heldSet = hidKeysHeld();

    for (auto button : buttons)
    {
        if (auto found = ::buttons.Find(button))
            return heldSet & (uint32_t)*found;
    }

    return false;
}
