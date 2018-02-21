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

void Gamepad::ClampAxis(float & x)
{
	if (abs(x) < 0.01)
		x = 0;
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