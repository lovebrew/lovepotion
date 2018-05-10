#include "common/runtime.h"
#include "objects/gamepad/gamepad.h"

Gamepad::Gamepad(int id, SDL_Joystick * joystick)
{
	this->id = id;

	this->joystickHandle = joystick;

	HidControllerType type = (HidControllerType)(TYPE_HANDHELD | TYPE_JOYCON_PAIR);
	
	//joycon pairing
	hidInitializeVibrationDevices(this->vibrationHandles[0], 2, (HidControllerID)id, type);

	//handheld mode
	//using hidGetHandheldMode as the index makes it easier to code this
	hidInitializeVibrationDevices(this->vibrationHandles[1], 2, CONTROLLER_HANDHELD, type);

	memset(this->vibration, 0, sizeof(this->vibration));

	this->vibrationDuration = -1;
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
	return this->joystickHandle != NULL;
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

	SDL_JoystickNumButtons(this->joystickHandle);
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

bool Gamepad::IsDown(const string & button)
{
	int numbuttons = this->GetButtonCount();

	SDL_JoystickUpdate();

	for (int i = 0; i < numbuttons; i++)
	{
		if (i < 0 || i >= numbuttons)
			continue;

		if (KEYS[i] != "" && KEYS[i] == button)
		{
			if (SDL_JoystickGetButton(this->joystickHandle, i) == 1)
				return true;
		}
	}

	return false;
}