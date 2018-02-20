#include "runtime.h"
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

vector<HidControllerJoystick> enums =
{
	JOYSTICK_LEFT,
	JOYSTICK_RIGHT
};

Gamepad::Gamepad(int id)
{
	this->id = id;

	printf("Connected controller %d\n", id + 1);
}

int Gamepad::GetID()
{
	return this->id;
}

HidControllerID Gamepad::GetInternalID()
{
	int indentity = this->id;
	if (this->id == 0)
		indentity = 10; //CONTROLLER_P1_AUTO
	
	return (HidControllerID)indentity;
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

void Gamepad::ScanAxes(pair<string, float> & data)
{
	HidControllerID id = this->GetInternalID();

	for (HidControllerJoystick joystickEnum : enums)
	{
		for (int i = 16; i < 24; i++)
		{
			hidJoystickRead(&this->stick, id, joystickEnum);

			data.first = BUTTONS[i];
			if (BUTTONS[i].substr(BUTTONS[i].length() - 1) == "x")
				data.second = ((float)this->stick.dx / JOYSTICK_MAX);
			else
				data.second = ((float)this->stick.dy / JOYSTICK_MAX);
		}
	}
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