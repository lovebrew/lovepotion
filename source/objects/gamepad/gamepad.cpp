#include "runtime.h"
#include <switch.h>
#include "gamepad.h"

vector<string> BUTTONS =
{
	"a", "b", "x", "y",
	"leftstick", "rightstick",
	"l", "r", "zl", "zr",
	"plus", "minus", "dpleft",
	"dpup", "dpright", "dpdown",
	"", "", "", "", "", "", "", "", //(16-23) sticks move for axis only
	"sl", "sr"
};

vector<Gamepad *> joycons;

Gamepad::Gamepad(int id)
{
	this->id = id;

	printf("Connected JoyCon %d\n", id);
}

bool Gamepad::IsDown(string button)
{
	u64 key = hidKeysDown((HidControllerID)this->id);

	for (uint i = 0; i < BUTTONS.size(); i ++)
	{
		if (key & BIT(i))
			return true;
	}

	return false;
}