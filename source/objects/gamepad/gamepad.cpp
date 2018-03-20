#include "common/runtime.h"
#include "gamepad.h"

vector<string> BUTTONS =
{
	"a", "b", "x", "y",
	"leftstick", "rightstick",
	"l", "r", "zl", "zr",
	"plus", "minus", "dpleft",
	"dpup", "dpright", "dpdown",
	"leftx", "lefty", "leftx", "lefty", 
	"rightx", "righty", "rightx", "righty", //(16-23) sticks move for axis only
	"sl", "sr"
};

Gamepad::Gamepad(int id)
{
	this->id = id;

	/*
	memset(this->vibrationValues, 0, sizeof(this->vibrationValues));
	memset(&this->vibrationValue_stop, 0, sizeof(HidVibrationValue));

	this->vibrationValue_stop.freq_low  = 160.0f;
	this->vibrationValue_stop.freq_high = 320.0f;

	hidInitializeVibrationDevices(this->vibrationDeviceHandles[0], 2, CONTROLLER_PLAYER_1, TYPE_HANDHELD | TYPE_JOYCON_PAIR);
	*/
}

int Gamepad::GetID()
{
	return this->id + 1;
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

string Gamepad::ScanButtons(bool isDown)
{
	HidControllerID id = this->GetInternalID();

	u64 keyDown = hidKeysDown(id);
	u64 keyUp = hidKeysUp(id);

	for (uint i = 0; i < BUTTONS.size(); i ++)
	{
		if (i < 16 || i > 23)
		{
			if ((keyDown & BIT(i)) && isDown)
				return BUTTONS[i];
			else if ((keyUp & BIT(i)) && !isDown)
				return BUTTONS[i];
		}
	}

	return "nil";
}

void Gamepad::ScanAxes(pair<string, float> & data, HidControllerJoystick joystick, int axis)
{
	HidControllerID id = this->GetInternalID();
	JoystickPosition position = this->sticks[0];

	if (joystick == JOYSTICK_RIGHT)
		position = this->sticks[1];

	hidJoystickRead(&position, id, joystick);

	if (axis == 0)
	{
		if (position.dx != 0)
		{
			if (joystick != JOYSTICK_RIGHT)
				data.first = "leftx";
			else
				data.first = "rightx";

			data.second = ((float)position.dx / JOYSTICK_MAX);
		}
	}
	else if (axis == 1)
	{
		if (position.dy != 0)
		{
			if (joystick != JOYSTICK_RIGHT)
				data.first = "lefty";
			else
				data.first = "righty";

			data.second = ((float)position.dy / JOYSTICK_MAX);
		}
	}

	this->ClampAxis(data.second);
}

bool Gamepad::GetAxis(float & value, int axis)
{
	bool valid = true;

	switch(axis)
	{
		case 1:
			value = this->ReadAxis(JOYSTICK_LEFT, true);
			break;
		case 2:
			value = this->ReadAxis(JOYSTICK_LEFT, false);
			break;
		case 4:
			value = this->ReadAxis(JOYSTICK_RIGHT, true);
			break;
		case 5:
			value = this->ReadAxis(JOYSTICK_RIGHT, false);
			break;
		default:
			value = 0.0f;
			valid = false;
			break;
	}

	return valid;
}

int Gamepad::GetButtonCount()
{
	return 0;
}

void Gamepad::SetVibration(float amp_low, float amp_high, float freq_low, float freq_high)
{
	/*
	this->vibrationValue.amp_low = amp_low;
	this->vibrationValue.amp_high = amp_high;

	this->vibrationValue.freq_low = freq_low;
	this->vibrationValue.freq_high = freq_high;

	memcpy(&this->vibrationValues[0], &this->vibrationValue, sizeof(HidVibrationValue));
	hidSendVibrationValues(VibrationDeviceHandles[0], this->vibrationValues, 2);
	*/
}

bool Gamepad::IsVibrationSupported()
{
	return true;
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

float Gamepad::ReadAxis(HidControllerJoystick joystick, bool horizontal)
{
	HidControllerID id = this->GetInternalID();

	JoystickPosition position;
	hidJoystickRead(&position, id, joystick);

	if (!horizontal)
		return ((float)position.dy / JOYSTICK_MAX);

	return ((float)position.dx / JOYSTICK_MAX);
}

bool Gamepad::IsDown(const string & button)
{
	u64 key = hidKeysDown((HidControllerID)this->id);

	for (uint i = 0; i < BUTTONS.size(); i ++)
	{
		if (key & BIT(i) && (button == BUTTONS[i]))
			return true;
	}

	return false;
}