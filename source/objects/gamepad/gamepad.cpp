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
			else if (keyUp & BIT(i) && !isDown)
				return BUTTONS[i];
		}
	}

	return "nil";
}

pair<string, float> Gamepad::ScanAxes()
{
	HidControllerID id = this->GetInternalID();

	JoystickPosition joystick = this->sticks[0];

	for (HidControllerJoystick joystickEnum : enums)
	{
		if (joystickEnum == JOYSTICK_RIGHT)
			joystick = this->sticks[1];

		for (int i = 16; i < 24; i++)
		{
			hidJoystickRead(&joystick, id, joystickEnum);

			if (BUTTONS[i].substr(BUTTONS[i].length() - 1) == "x")
				return std::make_pair(BUTTONS[i], ((float)joystick.dx / JOYSTICK_MAX));
			else
				return std::make_pair(BUTTONS[i], ((float)joystick.dy / JOYSTICK_MAX));
		}
	}

	return std::make_pair("nil", 0);
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