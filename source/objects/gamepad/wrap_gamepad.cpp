#include "runtime.h"
#include <switch.h>

#include "gamepad.h"
#include "wrap_gamepad.h"

#define CLASS_NAME "Joystick"
#define CLASS_TYPE LUAOBJ_TYPE_JOYSTICK

int gamepadNew(lua_State * L)
{
	return gamepadNew(L, 1);
}

int gamepadNew(lua_State * L, int id)
{
	void * raw_self = luaobj_newudata(L, sizeof(Gamepad));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	Gamepad * self = new (raw_self) Gamepad(id);

	joycons.push_back(self);
 
	printf("Joycon! %d", joycons.size());

	return 1;
}

int gamepadGC(lua_State * L)
{
	return 0;
}

int initGamepadClass(lua_State * L) 
{
	luaL_Reg reg[] = 
	{
		{"__gc",		gamepadGC	},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, gamepadNew, reg);

	return 1;
}