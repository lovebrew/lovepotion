#include "common/runtime.h"
#include "keyboard.h"

using love::Keyboard;

char BUTTONS[32][32] = {
	"a", "b", "select", "start",
	"right", "left", "up", "down",
	"rbutton", "lbutton", "x", "y",
	"", "", "lzbutton", "rzbutton",
	"", "", "", "",
	"touch", "", "", "",
	"cstickright", "cstickleft", "cstickup", "cstickdown",
	"cpadright", "cpadleft", "cpadup", "cpaddown"
};

int keyboardInit(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}