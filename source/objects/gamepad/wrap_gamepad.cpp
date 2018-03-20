#include "common/runtime.h"
#include <switch.h>

#include "objects/gamepad/gamepad.h"
#include "objects/gamepad/wrap_gamepad.h"

#define CLASS_NAME "Gamepad"
#define CLASS_TYPE LUAOBJ_TYPE_GAMEPAD

vector<Gamepad *> controllers;

int gamepadNew(lua_State * L)
{
	void * raw_self = luaobj_newudata(L, sizeof(Gamepad));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	Gamepad * self = new (raw_self) Gamepad(controllers.size());

	lua_getglobal(L, "__controllers"); //get global table

	lua_pushlightuserdata(L, self); //light userdata key
	lua_pushvalue(L, 2); //push the userdata value to the key
	lua_settable(L, -3); //set the taaable (╯°□°）╯︵ ┻━┻

	lua_setglobal(L, "__controllers");

	controllers.push_back(self);

	return 0;
}

int gamepadGetID(lua_State * L)
{
	Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushinteger(L, self->GetID());

	return 1;
}

int gamepadIsVibrationSupported(lua_State * L)
{
	Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushboolean(L, self->IsVibrationSupported());

	return 1;
}

int gamepadGetAxis(lua_State * L)
{
	Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	int axis = luaL_checkinteger(L, 2);

	float value;
	bool valid = self->GetAxis(value, axis);

	if (valid)
	{
		lua_pushnumber(L, value);

		return 1;
	}
	else
		return 0;
}

int gamepadGetButtonCount(lua_State * L)
{
	return 0;
}

int gamepadGetName(lua_State * L)
{
	Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushstring(L, self->GetName().c_str());

	return 1;
}

int gamepadIsDown(lua_State * L)
{
	Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	string button = string(luaL_checkstring(L, 2));

	lua_pushboolean(L, self->IsDown(button));

	return 1;
}

int gamepadSetVibration(lua_State * L)
{
	Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	float amp_low = luaL_optnumber(L, 2, 0);
	float freq_low = luaL_optnumber(L, 3, 160.0f);
	float amp_high = luaL_optnumber(L, 3, 0.0f);
	float freq_high = luaL_optnumber(L, 3, 320.0f);

	self->SetVibration(amp_low, freq_low, amp_high, freq_high);

	return 0;
}

int gamepadGC(lua_State * L)
{
	return 0;
}

int initGamepadClass(lua_State * L) 
{
	luaL_Reg reg[] = 
	{
		{"new",						gamepadNew					},
		{"getID",					gamepadGetID				},
		{"getName",					gamepadGetName				},
		{"getAxis",					gamepadGetAxis				},
		{"setVibration",			gamepadSetVibration			},
		{"isVibrationSupported",	gamepadIsVibrationSupported	},
		{"isDown",					gamepadIsDown				},
		{"__gc",					gamepadGC					},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, gamepadNew, reg);

	return 1;
}