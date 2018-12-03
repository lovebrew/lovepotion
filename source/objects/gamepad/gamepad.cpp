#include "common/runtime.h"
#include "objects/gamepad/gamepad.h"

Gamepad::Gamepad(int id) : Object("Joystick")
{
    this->id = id;

    HidControllerType type = (HidControllerType)(TYPE_HANDHELD | TYPE_JOYCON_PAIR);
    
    //joycon pairing
    hidInitializeVibrationDevices(this->vibrationHandles[0], 2, (HidControllerID)id, type);

    //handheld mode
    //using hidGetHandheldMode as the index makes it easier to code this
    hidInitializeVibrationDevices(this->vibrationHandles[1], 2, CONTROLLER_HANDHELD, type);

    memset(this->vibration, 0, sizeof(this->vibration));

    this->vibrationDuration = -1;

    this->layout = "dual";

    SDL_Joystick * joystick = SDL_JoystickOpen(id);
    this->joysticks.first = joystick;
    this->joysticks.second = nullptr;

    this->joycon = {CONTROLLER_IDS[id], CONTROLLER_IDS[id + 1]};
}

int Gamepad::GetID()
{
    return this->id + 1;
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

HidControllerID Gamepad::GetInternalID()
{
    int indentity = this->id;
    if (this->id == 0)
        indentity = 10; //CONTROLLER_P1_AUTO
    
    return (HidControllerID)indentity;
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

    hidSendVibrationValues(this->vibrationHandles[hidGetHandheldMode()], this->vibration, 2);
}

string Gamepad::GetName()
{
    HidControllerID id = this->GetInternalID();

    HidControllerLayoutType type = hidGetControllerLayout(id);

    switch(type)
    {
        case LAYOUT_PROCONTROLLER:
            return "Pro Controller";
        case LAYOUT_HANDHELD:
            return "Handheld";
        case LAYOUT_SINGLE:
            return "Joycon";
        case LAYOUT_LEFT:
            return "Left Joycon";
        case LAYOUT_RIGHT:
            return "Right Joycon";
        default: //this shouldn't happen
            return "nil";
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

    if (this->layout == "dual" && layout == "single")
    {
        for (auto item : this->joycon)
            hidSetNpadJoyAssignmentModeSingleByDefault(item);

        this->joysticks.first = SDL_JoystickOpen(this->id);
        this->joysticks.second = SDL_JoystickOpen(this->id + 1);
    }
    else if (this->layout == "single" && layout == "dual")
    {
        for (auto item : this->joycon)
            hidSetNpadJoyAssignmentModeDual(item);

        hidMergeSingleJoyAsDualJoy(this->joycon[0], this->joycon[1]);

        this->joysticks.first = SDL_JoystickOpen(this->id);
        this->joysticks.second = nullptr;
    }

    this->layout = layout;
}

bool Gamepad::IsDown(uint button)
{
    hidScanInput();

    u64 heldButton = hidKeysHeld(this->GetInternalID());
    bool keyDown = false;

    for (uint i = 0; i < KEYS.size(); i++)
    {
        if (heldButton & BIT(i) && button == i)
            keyDown = true;
    }

    return keyDown;
}

bool Gamepad::IsGamepadDown(const string & button)
{
    hidScanInput();

    u64 heldButton = hidKeysHeld(this->GetInternalID());
    bool keyDown = false;

    for (uint i = 0; i < KEYS.size(); i++)
    {
        if (KEYS[i] != "")
        {
            if (heldButton & BIT(i) && button == KEYS[i])
                keyDown = true;
        }
    }

    return keyDown;
}

float Gamepad::GetAxis(uint axis)
{
    float value = 0;

    return value;
}

float Gamepad::GetGamepadAxis(const string & axis)
{
    float value = 0;

    return value;
}