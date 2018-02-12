#include "runtime.h"
#include <switch.h>

#include "gamepad.h"
#include "wrap_gamepad.h"

#define CLASS_NAME "Gamepad"
#define CLASS_TYPE LUAOBJ_TYPE_GAMEPAD

vector<Gamepad *> controllers;

int gamepadNew(lua_State * L)
{
	void * raw_self = luaobj_newudata(L, sizeof(Gamepad));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	Gamepad * self = new (raw_self) Gamepad(controllers.size());

	controllers.push_back(self);

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
		{"new",			gamepadNew	},
		{"__gc",		gamepadGC	},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, gamepadNew, reg);

	return 1;
}