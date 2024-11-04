#include <objects/gamepad.hpp>
#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <modules/joystickmodule_ext.hpp>
#include <modules/timer_ext.hpp>

#include <utilities/result.hpp>
#include <utilities/wpad.hpp>

using namespace love;

#define Module() (Module::GetInstance<JoystickModule<Console::Which>>(Module::M_JOYSTICK))

// clang-format off
constexpr BidirectionalMap buttons = {
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

constexpr BidirectionalMap axes = {
    Joystick<>::GAMEPAD_AXIS_LEFTX,        VPAD_STICK_L_EMULATION_LEFT | VPAD_STICK_L_EMULATION_RIGHT,
    Joystick<>::GAMEPAD_AXIS_LEFTY,        VPAD_STICK_L_EMULATION_UP | VPAD_STICK_L_EMULATION_DOWN,
    Joystick<>::GAMEPAD_AXIS_RIGHTX,       VPAD_STICK_R_EMULATION_LEFT | VPAD_STICK_R_EMULATION_RIGHT,
    Joystick<>::GAMEPAD_AXIS_RIGHTY,       VPAD_STICK_R_EMULATION_UP | VPAD_STICK_R_EMULATION_DOWN,
    Joystick<>::GAMEPAD_AXIS_TRIGGERLEFT,  VPAD_BUTTON_ZL,
    Joystick<>::GAMEPAD_AXIS_TRIGGERRIGHT, VPAD_BUTTON_ZR
};
// clang-format on

Gamepad::Gamepad(int id) : state {}
{}

Gamepad::Gamepad(int id, int index) : Gamepad(id)
{
    this->Open(index);
}

bool Gamepad::Open(int index)
{
    this->Close();

    this->name = guid::GetGamepadName(this->GetGamepadType());
    this->guid = guid::GetGamepadGUID(this->GetGamepadType());

    this->instanceId = index;

    return this->IsConnected();
}

void Gamepad::Update()
{
    VPADReadError error {};
    VPADRead(VPAD_CHAN_0, &this->state, 1, &error);

    std::fill_n(this->triggers, 0x2, Trigger {});

    if (error == VPAD_READ_NO_SAMPLES)
        return;

    this->buttonStates.pressed  = this->state.trigger;
    this->buttonStates.released = this->state.release;
    this->buttonStates.held     = this->state.hold;

    this->leftStick  = { this->state.leftStick.x, this->state.leftStick.y };
    this->rightStick = { this->state.rightStick.x, this->state.rightStick.y };

    if (this->buttonStates.held & VPAD_BUTTON_ZL)
        this->triggers[0x0].down = true;

    if (this->buttonStates.held & VPAD_BUTTON_ZR)
        this->triggers[0x1].down = true;

    // clang-format off
    if (this->IsSensorEnabled(Sensor::SENSOR_ACCELEROMETER))
        ((Accelerometer*)this->sensors[Sensor::SENSOR_ACCELEROMETER])->Update(this->state.accelorometer.acc);

    if (this->IsSensorEnabled(Sensor::SENSOR_GYROSCOPE))
        ((Gyroscope*)this->sensors[Sensor::SENSOR_GYROSCOPE])->Update(this->state.gyro);
    // clang-format on
}

bool Gamepad::IsDown(JoystickInput& result)
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

bool Gamepad::IsDown(const std::vector<int>& inputs) const
{
    if (!this->IsConnected())
        return false;

    int count    = this->GetButtonCount();
    auto records = buttons.GetEntries();

    for (int button : inputs)
    {
        if (button < 0 || button >= count)
            continue;

        if (this->buttonStates.held & records[button].second)
            return true;
    }

    return false;
}

bool Gamepad::IsGamepadDown(const std::vector<GamepadButton>& inputs) const
{
    uint32_t heldSet = this->buttonStates.held;

    for (auto button : inputs)
    {
        if (auto found = buttons.Find(button))
            return heldSet & (uint32_t)*found;
    }

    return false;
}

bool Gamepad::IsUp(JoystickInput& result)
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

float Gamepad::GetAxis(int index)
{
    if (index == 0 || index == 1)
        return (index == 1) ? this->leftStick.dx : this->leftStick.dy;
    else if (index == 2 || index == 3)
        return (index == 3) ? this->rightStick.dx : this->rightStick.dy;
    else if (index == 4)
        return this->triggers[0x0].down;
    else if (index == 5)
        return this->triggers[0x1].down;

    return 0.0f;
}

/* internal use for the callback */
bool Gamepad::IsAxisChanged(GamepadAxis axis)
{
    auto vpadAxis = *axes.Find(axis);

    if (vpadAxis & this->buttonStates.held)
    {
        this->buttonStates.held ^= vpadAxis;
        return true;
    }

    if (vpadAxis & this->buttonStates.released)
    {
        this->buttonStates.released ^= vpadAxis;
        return true;
    }

    return false;
}

float Gamepad::GetGamepadAxis(GamepadAxis axis)
{
    if (!this->IsConnected())
        return 0.0f;

    return this->GetAxis(axis);
}

std::vector<float> Gamepad::GetAxes()
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

bool Gamepad::SetVibration(float left, float right, float duration)
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

bool Gamepad::SetVibration()
{
    return this->vibration.Stop();
}

void Gamepad::GetVibration(float& left, float& right)
{
    this->vibration.GetValues(left, right);
}

bool Gamepad::HasSensor(Sensor::SensorType type) const
{
    return true;
}

bool Gamepad::IsSensorEnabled(Sensor::SensorType type)
{
    return this->sensors[type];
}

void Gamepad::SetSensorEnabled(Sensor::SensorType type, bool enabled)
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

std::vector<float> Gamepad::GetSensorData(Sensor::SensorType type)
{
    if (!this->IsSensorEnabled(type))
    {
        auto name = Sensor::sensorTypes.ReverseFind(type);
        throw love::Exception("\"%s\" sensor is not enabled", *name);
    }

    return this->sensors[type]->GetData();
}
