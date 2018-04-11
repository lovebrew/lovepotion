#include "common/runtime.h"

#include "objects/gamepad/gamepad.h"
#include "objects/gamepad/wrap_gamepad.h"
#include "modules/joystick.h"

void Joystick::Initialize(lua_State * L)
{
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	
	gamepadNew(L);
}

Gamepad * Joystick::GetJoystickFromID(int id)
{
	return controllers[id];
}

//Löve2D Functions

//Super hacky workaround for now..
//love.joystick.getJoysticks
int Joystick::GetJoysticks(lua_State * L)
{
	lua_newtable(L);

	for (uint i = 0; i < controllers.size(); i++)
	{
		lua_pushnumber(L, i + 1);
		
		lua_getglobal(L, "__controllers");
		lua_pushlightuserdata(L, controllers[i]);
		lua_gettable(L, -2);
		lua_remove(L, -2);

		lua_settable(L, -3);
	}

	return 1;
}


//love.joystick.getJoystickCount
int Joystick::GetJoystickCount(lua_State * L)
{
	return 0;
}

//love.joystick.setJoyconMode
int Joystick::SetJoyconMode(lua_State * L)
{
	return 0;
}

//End Löve2D Functions

int Joystick::Register(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "getJoysticks",			GetJoysticks	},
		{ "getJoystickCount",		GetJoystickCount},
		{ "setJoyconMode",			SetJoyconMode	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}