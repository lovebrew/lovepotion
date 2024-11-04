#include <objects/procontroller.hpp>
#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <modules/joystickmodule_ext.hpp>
#include <modules/timer_ext.hpp>

#include <utilities/result.hpp>
#include <utilities/wpad.hpp>

using namespace love;

#define Module() (Module::GetInstance<JoystickModule<Console::Which>>(Module::M_JOYSTICK))

// clang-format off
constexpr BidirectionalMap buttons = {
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

constexpr BidirectionalMap axes = {
    Joystick<>::GAMEPAD_AXIS_LEFTX,  WPAD_PRO_STICK_L_EMULATION_LEFT | WPAD_PRO_STICK_L_EMULATION_RIGHT,
    Joystick<>::GAMEPAD_AXIS_LEFTY,  WPAD_PRO_STICK_L_EMULATION_UP   | WPAD_PRO_STICK_L_EMULATION_DOWN,
    Joystick<>::GAMEPAD_AXIS_RIGHTX, WPAD_PRO_STICK_R_EMULATION_LEFT | WPAD_PRO_STICK_R_EMULATION_RIGHT,
    Joystick<>::GAMEPAD_AXIS_RIGHTY, WPAD_PRO_STICK_R_EMULATION_UP   | WPAD_PRO_STICK_R_EMULATION_DOWN,
};
// clang-format on

ProController::ProController(int id) : state {}
{}

ProController::ProController(int id, int index) : ProController(id)
{
    this->Open(index);
}

bool ProController::Open(int index)
{
    this->Close();

    this->name = guid::GetGamepadName(this->GetGamepadType());
    this->guid = guid::GetGamepadGUID(this->GetGamepadType());

    this->instanceId = index;

    return this->IsConnected();
}

void ProController::Update()
{
    KPADError error {};
    KPADReadEx((WPADChan)(this->instanceId - 1), &this->state, 1, &error);

    std::fill_n(this->triggers, 0x2, Trigger {});

    if (error == KPAD_ERROR_NO_SAMPLES)
        return;

    this->buttonStates.pressed  = this->state.trigger;
    this->buttonStates.released = this->state.release;
    this->buttonStates.held     = this->state.hold;

    this->leftStick  = { this->state.pro.leftStick.x, this->state.pro.leftStick.y };
    this->rightStick = { this->state.pro.rightStick.x, this->state.pro.rightStick.y };

    if (this->buttonStates.held & WPAD_PRO_TRIGGER_ZL)
        this->triggers[0x0].down = true;

    if (this->buttonStates.held & WPAD_PRO_TRIGGER_ZR)
        this->triggers[0x1].down = true;
}

bool ProController::IsDown(JoystickInput& result)
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

bool ProController::IsDown(const std::vector<int>& inputs) const
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

bool ProController::IsGamepadDown(const std::vector<GamepadButton>& inputs) const
{
    uint32_t heldSet = this->buttonStates.held;

    for (auto button : inputs)
    {
        if (auto found = buttons.Find(button))
            return heldSet & (uint32_t)*found;
    }

    return false;
}

bool ProController::IsUp(JoystickInput& result)
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

float ProController::GetAxis(int index)
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
bool ProController::IsAxisChanged(GamepadAxis axis)
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

float ProController::GetGamepadAxis(GamepadAxis axis)
{
    if (!this->IsConnected())
        return 0.0f;

    return this->GetAxis(axis);
}

std::vector<float> ProController::GetAxes()
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

bool ProController::SetVibration(float left, float right, float duration)
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

bool ProController::SetVibration()
{
    return this->vibration.Stop();
}

void ProController::GetVibration(float& left, float& right)
{
    this->vibration.GetValues(left, right);
}

bool ProController::HasSensor(Sensor::SensorType type) const
{
    return false;
}

bool ProController::IsSensorEnabled(Sensor::SensorType type)
{
    return this->sensors[type];
}

void ProController::SetSensorEnabled(Sensor::SensorType type, bool enabled)
{
    if (this->sensors[type] && !enabled)
        this->sensors[type] = nullptr;
    else if (this->sensors[type] == nullptr && enabled)
    {
        if (this->HasSensor(type))
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
}

std::vector<float> ProController::GetSensorData(Sensor::SensorType type)
{
    if (!this->IsSensorEnabled(type))
    {
        auto name = Sensor::sensorTypes.ReverseFind(type);
        throw love::Exception("\"%s\" sensor is not enabled", *name);
    }

    return this->sensors[type]->GetData();
}
