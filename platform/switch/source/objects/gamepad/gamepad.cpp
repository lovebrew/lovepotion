#include "common/runtime.h"
#include "objects/gamepad/gamepad.h"

map<string, map<string, int>> HANDLE_COUNT = 
{
    { "Joy-Con Pair", { 
        { "Vibration", 2 },
        { "SixAxis",   2 }
    }},

    { "Pro Controller", {
        { "Vibration", 1 },
        { "SixAxis",   1 }
    }},

    { "Handheld", {
        { "Vibration", 2 },
        { "SixAxis",   1 }
    }}
};

Gamepad::Gamepad() : Object("Joystick")
{
    this->id = 0;

    this->vibrationDuration = -1;

    this->split = false;

    SDL_Joystick * joystick = SDL_JoystickOpen(this->id);
    this->joysticks.first = joystick;
    this->joysticks.second = nullptr;

    this->joycon = {CONTROLLER_IDS[this->id], CONTROLLER_IDS[this->id + 1]};

    this->InitializeVibration();
    this->InitializeSixAxis();
}

Gamepad::~Gamepad()
{
    string name = this->GetName();

    hidStopSixAxisSensor(this->sixAxisHandles[0]);
    if (name == "Joy-Con Pair")
        hidStopSixAxisSensor(this->sixAxisHandles[1]);
}

void Gamepad::InitializeVibration()
{
    string name = this->GetName();
    
    if (name == "Joy-Con Pair")
        hidInitializeVibrationDevices(this->vibrationHandles[0], 2, CONTROLLER_IDS[this->id], TYPE_JOYCON_PAIR);
    else if (name == "Pro Controller")
        hidInitializeVibrationDevices(this->vibrationHandles[0], 1, CONTROLLER_IDS[this->id], TYPE_PROCONTROLLER);
    else if (name == "Handheld")
        hidInitializeVibrationDevices(this->vibrationHandles[0], 2, CONTROLLER_HANDHELD, TYPE_HANDHELD);

    memset(this->vibration, 0, sizeof(this->vibration));
}

void Gamepad::InitializeSixAxis()
{
    string name = this->GetName();
    
    if (name == "Joy-Con Pair")
        hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 2, CONTROLLER_IDS[this->id], TYPE_JOYCON_PAIR);
    else if (name == "Pro Controller")
        hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 1, CONTROLLER_IDS[this->id], TYPE_PROCONTROLLER);
    else if (name == "Handheld")
        hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 1, CONTROLLER_HANDHELD, TYPE_HANDHELD);

    hidStartSixAxisSensor(this->sixAxisHandles[0]);
    if (name == "Joy-Con Pair")
        hidStartSixAxisSensor(this->sixAxisHandles[1]);
}

int Gamepad::GetID()
{
    return this->id;
}

void Gamepad::Update(float delta)
{
    if (this->vibrationDuration == -1)
        return;

    if (this->vibrationDuration >= 0)
        this->vibrationDuration -= delta;
    else
        this->SetVibration(0, 0, -1);
}

bool Gamepad::IsConnected()
{
    return true;
}

//Useful only for certain things--
HidControllerID Gamepad::GetInternalID()
{
    if (this->id == 0)
        return CONTROLLER_IDS[8];

    return CONTROLLER_IDS[this->id];
}

void Gamepad::ClampAxis(float & x)
{
    if (abs(x) < 0.01)
        x = 0;
}

int Gamepad::GetButtonCount()
{
    if (!this->IsConnected())
        return 0;

    return KEYS.size() - 8;
}

bool Gamepad::IsVibrationSupported()
{
    return true;
}

void Gamepad::SetVibrationData(HidVibrationValue & vibration, double amplitude)
{
    vibration.freq_low = 160.0f;
    vibration.freq_high = 320.0f;

    vibration.amp_low = amplitude;
    vibration.amp_high = amplitude;
}

void Gamepad::SetVibration(double left, double right, double duration)
{
    this->vibrationDuration = duration;

    double leftAmplitude = clamp(0, left, 1);
    HidVibrationValue leftValue;

    this->SetVibrationData(leftValue, leftAmplitude);
    memcpy(&this->vibration[0], &leftValue, sizeof(HidVibrationValue));

    double rightAmplitude = clamp(0, right, 1);
    HidVibrationValue rightValue;
    
    this->SetVibrationData(rightValue, rightAmplitude);
    memcpy(&this->vibration[1], &rightValue, sizeof(HidVibrationValue));

    string name = this->GetName();
    hidSendVibrationValues(this->vibrationHandles[0], this->vibration, HANDLE_COUNT[name]["Vibration"]);
}

string Gamepad::GetName()
{
    HidControllerType type = hidGetControllerType(CONTROLLER_IDS[this->id]);

    switch(type)
    {
        case TYPE_PROCONTROLLER:
            return "Pro Controller";
        case TYPE_JOYCON_PAIR:
            return "Joy-Con";
        case TYPE_JOYCON_LEFT:
            return "Left Joy-Con";
        case TYPE_JOYCON_RIGHT:
            return "Right Joy-Con";
        default: //this shouldn't happen
            return "Handheld";
    }
}

void Gamepad::SetLayout(const string & mode, const string & holdType)
{
    string name = this->GetName();
    if (name == "Handheld" || name == "Pro Controller")
        return;

    if (this->joysticks.first != nullptr)
        SDL_JoystickClose(this->joysticks.first);

    if (this->joysticks.second != nullptr)
        SDL_JoystickClose(this->joysticks.second);

    this->joysticks = std::make_pair(nullptr, nullptr);

    if (!this->split && mode == "single")
    {
        for (auto item : this->joycon)
            hidSetNpadJoyAssignmentModeSingleByDefault(item);

        this->joysticks.first = SDL_JoystickOpen(this->id);
        this->joysticks.second = SDL_JoystickOpen(this->id + 1);

        //hidSetNpadJoyHoldType(HidJoyHoldType_Horizontal);
    }
    else if (this->split && mode == "dual")
    {
        for (auto item : this->joycon)
            hidSetNpadJoyAssignmentModeDual(item);

        hidMergeSingleJoyAsDualJoy(this->joycon[0], this->joycon[1]);

        this->joysticks.first = SDL_JoystickOpen(this->id);
        this->joysticks.second = nullptr;

        //hidSetNpadJoyHoldType(HidJoyHoldType_Default);
    }

    this->split = (mode == "single");
}

bool Gamepad::IsDown(uint button)
{
    return SDL_JoystickGetButton(this->joysticks.first, button - 1);
}

bool Gamepad::IsGamepadDown(const string & button)
{
    bool padDown = false;

    for (uint buttonIndex = 0; buttonIndex < KEYS.size(); buttonIndex++)
    {
        if (KEYS[buttonIndex] != "")
        {
            padDown = SDL_JoystickGetButton(this->joysticks.first, buttonIndex);
            if (this->split && !padDown)
                padDown = SDL_JoystickGetButton(this->joysticks.second, buttonIndex);

            if (KEYS[buttonIndex] == button)
                break;
        }
    }

    return padDown;
}

float Gamepad::GetAxis(uint axis)
{
    SixAxisSensorValues axisValues;
    string name = this->GetName();

    hidSixAxisSensorValuesRead(&axisValues, this->GetInternalID(), 1);

    if (axis >= 4 && axis < 7)
    {
        if (axis == 4)
            return axisValues.accelerometer.x;
        else if (axis == 5)
            return axisValues.accelerometer.y;
        
        return axisValues.accelerometer.z;
    }
    
    if (axis >= 7 && axis < 10)
    {
        if (axis == 7)
            return axisValues.gyroscope.x;
        else if (axis == 8)
            return axisValues.gyroscope.y;

        return axisValues.gyroscope.z;
    }

    if (axis >= 10 && axis < 13)
    {
        if (axis == 10)
            return axisValues.unk.x;
        else if (axis == 11)
            return axisValues.unk.y;

        return axisValues.unk.z;
    }

    float value = SDL_JoystickGetAxis(this->joysticks.first, axis);

    return value / JOYSTICK_MAX;
}

float Gamepad::GetGamepadAxis(const string & axis)
{
    float value = 0;
  
    if (axis == "leftx")
        value = SDL_JoystickGetAxis(this->joysticks.first, 0);
    else if (axis == "lefty")
        value = SDL_JoystickGetAxis(this->joysticks.first, 1);
    else if (axis == "rightx")
        value = SDL_JoystickGetAxis(this->joysticks.first, 2);
    else if (axis == "righty")
        value = SDL_JoystickGetAxis(this->joysticks.first, 3);
  
    return value / JOYSTICK_MAX;
}