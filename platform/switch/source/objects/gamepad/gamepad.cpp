#include "common/runtime.h"
#include "objects/gamepad/gamepad.h"

map<string, map<string, int>> HANDLE_COUNT = 
{
    { "Joy-Con Pair", { 
        { "Vibration", 2 },
        { "SixAxis",   2 }
    }},

    { "Left Joy-Con", {
        { "Vibration", 1 },
        { "SixAxis",   1 }
    }},

    { "Right Joy-Con", {
        { "Vibration", 1 },
        { "SixAxis",   1 }
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
    this->id = controllers.size();

    this->vibrationDuration = -1;

    SDL_Joystick * joystick = SDL_JoystickOpen(this->GetID());
    this->joystick = joystick;

    this->joycon = {CONTROLLER_IDS[this->GetID()], CONTROLLER_IDS[this->GetID() + 1]};

    this->InitializeVibration();
    this->InitializeSixAxis();
}

Gamepad::~Gamepad()
{
    string name = this->GetName();

    hidStopSixAxisSensor(this->sixAxisHandles[0]);
    if (name == "Joy-Con Pair")
        hidStopSixAxisSensor(this->sixAxisHandles[1]);

    SDL_JoystickClose(this->joystick);
}

void Gamepad::InitializeVibration()
{
    string name = this->GetName();

    // Reset things because of when we split/join
    memset(this->vibration, 0, sizeof(this->vibration));
    memset(this->vibrationHandles, 0, sizeof(this->vibrationHandles));
    
    int identity = this->GetID();

    if (name == "Joy-Con Pair")
        hidInitializeVibrationDevices(this->vibrationHandles[0], 2, CONTROLLER_IDS[identity], TYPE_JOYCON_PAIR);
    else if (name == "Pro Controller")
        hidInitializeVibrationDevices(this->vibrationHandles[0], 1, CONTROLLER_IDS[identity], TYPE_PROCONTROLLER);
    else if (name == "Handheld")
        hidInitializeVibrationDevices(this->vibrationHandles[0], 2, CONTROLLER_HANDHELD, TYPE_HANDHELD);
    else if (name == "Left Joy-Con")
        hidInitializeVibrationDevices(this->vibrationHandles[0], 1, CONTROLLER_IDS[identity], TYPE_JOYCON_LEFT);
    else if (name == "Right Joy-Con")
        hidInitializeVibrationDevices(this->vibrationHandles[0], 1, CONTROLLER_IDS[identity], TYPE_JOYCON_RIGHT);
}

void Gamepad::InitializeSixAxis()
{
    string name = this->GetName();
    
    // Reset things because of when we split/join
    memset(this->sixAxisHandles, 0, sizeof(this->sixAxisHandles));

    int identity = this->GetID();

    if (name == "Joy-Con Pair")
        hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 2, CONTROLLER_IDS[identity], TYPE_JOYCON_PAIR);
    else if (name == "Pro Controller")
        hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 1, CONTROLLER_IDS[identity], TYPE_PROCONTROLLER);
    else if (name == "Handheld")
        hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 1, CONTROLLER_HANDHELD, TYPE_HANDHELD);
    else if (name == "Left Joy-Con")
        hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 1, CONTROLLER_IDS[identity], TYPE_JOYCON_LEFT);
    else if (name == "Right Joy-Con")
        hidGetSixAxisSensorHandles(&this->sixAxisHandles[0], 1, CONTROLLER_IDS[identity], TYPE_JOYCON_RIGHT);

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

    return SDL_JoystickNumButtons(this->joystick);
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
        default:
            return "Handheld";
    }
}

void Gamepad::SetSplit(bool split)
{
    this->split = split;
}

bool Gamepad::Split(const string & holdType)
{
    string name = this->GetName();
    if (name == "Handheld" || name == "Pro Controller")
        return false;

    if (!this->split)
    {
        for (auto item : this->joycon)
            hidSetNpadJoyAssignmentModeSingleByDefault(item);

        HidJoyHoldType hold = HidJoyHoldType_Horizontal;
        if (holdType == "vertical")
            hold = HidJoyHoldType_Default;

        hidSetNpadJoyHoldType(hold);
        
        this->InitializeSixAxis();
        this->InitializeVibration();

        this->split = true;

        return true;
    }

    return false;
}

bool Gamepad::Merge(Gamepad * other)
{
    string name = this->GetName();
    if (name == "Handheld" || name == "Pro Controller")
        return false;

    if (this->split)
    {
        for (auto item : this->joycon)
            hidSetNpadJoyAssignmentModeDual(item);

        if (hidGetControllerType(CONTROLLER_IDS[this->GetID()]) & TYPE_JOYCON_LEFT)
        {
            if (hidGetControllerType(CONTROLLER_IDS[other->GetID()]) & TYPE_JOYCON_RIGHT)
            {
                if (this->GetID() < other->GetID())
                    hidMergeSingleJoyAsDualJoy(CONTROLLER_IDS[this->GetID()], CONTROLLER_IDS[other->GetID()]);
                else
                    hidMergeSingleJoyAsDualJoy(CONTROLLER_IDS[other->GetID()], CONTROLLER_IDS[this->GetID()]);
            }
        }

        hidSetNpadJoyHoldType(HidJoyHoldType_Default);

        this->InitializeSixAxis();
        this->InitializeVibration();

        this->split = false;

        return true;
    }

    return false;
}

bool Gamepad::IsDown(uint button)
{
    // Exclude joystick movement
    if (button > 15 && button < 24)
        return false;

    return SDL_JoystickGetButton(this->joystick, button);
}

bool Gamepad::IsGamepadDown(const string & button)
{
    bool padDown = false;

    for (uint buttonIndex = 0; buttonIndex < KEYS.size(); buttonIndex++)
    {
        if (KEYS[buttonIndex] != "")
        {
            padDown = SDL_JoystickGetButton(this->joystick, buttonIndex);

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

    float value = SDL_JoystickGetAxis(this->joystick, axis);

    return value / JOYSTICK_MAX;
}

float Gamepad::GetGamepadAxis(const string & axis)
{
    float value = 0;
  
    if (axis == "leftx")
        value = SDL_JoystickGetAxis(this->joystick, 0);
    else if (axis == "lefty")
        value = SDL_JoystickGetAxis(this->joystick, 1);
    else if (axis == "rightx")
        value = SDL_JoystickGetAxis(this->joystick, 2);
    else if (axis == "righty")
        value = SDL_JoystickGetAxis(this->joystick, 3);
  
    return value / JOYSTICK_MAX;
}