#include "common/runtime.h"

#include "modules/joystick.h"
#include "objects/gamepad/gamepad.h"

void Joystick::Initialize()
{

}

//Löve2D Functions

//Super hacky workaround for now..
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

int Joystick::GetJoystickCount(lua_State * L)
{
	return 0;
}

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