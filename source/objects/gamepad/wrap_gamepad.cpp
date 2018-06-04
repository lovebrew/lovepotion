#include "common/runtime.h"

#include "objects/gamepad/gamepad.h"
#include "objects/gamepad/wrap_gamepad.h"

#define CLASS_NAME "Gamepad"
#define CLASS_TYPE LUAOBJ_TYPE_GAMEPAD

vector<Gamepad *> controllers;

int gamepadNew(lua_State * L)
{
	void * raw_self = luaobj_newudata(L, sizeof(Gamepad));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	SDL_JoystickOpen(0);

	Gamepad * self = new (raw_self) Gamepad(controllers.size());

	love_register(L, 2, self);

	controllers.push_back(self);

	return 0;
}

//Gamepad:getID
int gamepadGetID(lua_State * L)
{
	Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushinteger(L, self->GetID());

	return 1;
}

//Gamepad:isVibrationSupported
int gamepadIsVibrationSupported(lua_State * L)
{
	Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushboolean(L, self->IsVibrationSupported());

	return 1;
}

//Gamepad:getAxis
int gamepadGetAxis(lua_State * L)
{
	Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	return 0;
}

//Gamepad:getButtonCount
int gamepadGetButtonCount(lua_State * L)
{
	Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushinteger(L, self->GetButtonCount());

	return 1;
}

//Gamepad:getName
int gamepadGetName(lua_State * L)
{
	Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushstring(L, self->GetName().c_str());

	return 1;
}

//Gamepad:isGamepadDown
int gamepadIsGamepadDown(lua_State * L)
{
	Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	string button = string(luaL_checkstring(L, 2));

	lua_pushboolean(L, self->IsGamepadDown(button));

	return 1;
}

//Gamepad:isetLayout
int gamepadSetLayout(lua_State * L)
{
	Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	string layout = string(luaL_checkstring(L, 2));

	self->SetLayout(layout);

	return 1;
}

//Gamepad:isDown
int gamepadIsDown(lua_State * L)
{
	Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	int button = luaL_checkinteger(L, 2);

	lua_pushboolean(L, self->IsDown(button));

	return 1;
}

//Gamepad:setVibration
int gamepadSetVibration(lua_State * L)
{
	Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	double left = 0;
	double right = 0;

	if (!lua_isnoneornil(L, 2))
	{
		left = luaL_checknumber(L, 2);
		right = luaL_checknumber(L, 3);
	}

	double duration = luaL_optnumber(L, 4, -1);

	self->SetVibration(left, right, duration);

	return 0;
}

int gamepadToString(lua_State * L)
{
	Gamepad * self = (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	char * data = self->ToString(CLASS_NAME);

	lua_pushstring(L, data);

	free(data);

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
		{"new",						gamepadNew					},
		{"getID",					gamepadGetID				},
		{"getName",					gamepadGetName				},
		{"getAxis",					gamepadGetAxis				},
		{"setVibration",			gamepadSetVibration			},
		{"isVibrationSupported",	gamepadIsVibrationSupported	},
		{"isGamepadDown",			gamepadIsGamepadDown		},
		{"setLayout",				gamepadSetLayout			},
		{"isDown",					gamepadIsDown				},
		{"__gc",					gamepadGC					},
		{"__tostring",				gamepadToString				},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, gamepadNew, reg);

	return 1;
}