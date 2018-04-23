#include "common/runtime.h"
#include "objects/gamepad/gamepad.h"

Gamepad::Gamepad(int id, SDL_Joystick * joystick)
{
	this->id = id;

	this->joystickHandle = joystick;
	hidInitializeVibrationDevices(this->vibrationHandles, 2, (HidControllerID)id, TYPE_JOYCON_PAIR);

	memset(this->vibration, 0, sizeof(this->vibration));

	this->vibration[0].freq_low  = 10.0f;
	this->vibration[0].freq_high = 10.0f;

	this->vibration[1].freq_low  = 10.0f;
	this->vibration[1].freq_high = 10.0f;
}

int Gamepad::GetID()
{
	return this->id + 1;
}

bool Gamepad::IsConnected()
{
	return this->joystickHandle != NULL && SDL_JoystickGetAttached(this->joystickHandle);
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

	return SDL_JoystickNumButtons(this->joystickHandle);
}

bool Gamepad::IsVibrationSupported()
{
	return true;
}

void Gamepad::SetVibration(double left, double right)
{
	double leftAmplitude = clamp(0, left, 1);
	this->vibration[0].amp_low   = leftAmplitude;
	this->vibration[0].amp_high  = leftAmplitude;

	double rightAmplitude = clamp(0, right, 1);
	this->vibration[1].amp_low   = rightAmplitude;
	this->vibration[1].amp_high  = rightAmplitude;
	
	hidSendVibrationValues(this->vibrationHandles, this->vibration, 2);
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
	/*int numbuttons = this->GetButtonCount();

	for (int i = 0; i < KEYS.size(); i++)
	{
		if (i < 0 || i >= numbuttons)
			continue;

		if (KEYS[i] != "" && KEYS[i] == button)
		{
			if (SDL_JoystickGetButton(this->joystickHandle, i) == 1)
				return true;
		}
	}*/

	return false;
}