#include "runtime.h"
#include "system.h"

void System::Initialize()
{
	/*
	** Include things like configuration
	** Account name that runs the game
	** Battery info (power, state)
	** Region and language
	*/
}

//Löve2D Functions

//love.system.getOS
int System::GetOS(lua_State * L)
{
	lua_pushstring(L, "HorizonNX");

	return 1;
}

//love.system.getProcessorCount
int System::GetProcessorCount(lua_State * L)
{
	lua_pushnumber(L, 4);

	return 1;
}

//love.system.getPowerInfo
int System::GetPowerInfo(lua_State * L)
{	
	return 0;
}

//love.system.getRegion
int System::GetRegion(lua_State * L)
{
	return 0;
}

//love.system.getLanguage
int System::GetLanguage(lua_State * L)
{
	return 0;
}

//love.system.getWifiStrength
int System::GetWifiStrength(lua_State * L)
{
	return 0;
}

//love.system.hasWifiConnection
int System::HasWifiConnection(lua_State * L)
{
	return 0;
}

//love.system.getUsername
int System::GetUsername(lua_State * L)
{
	return 0;
}

//End Löve2D Functions

void System::Exit()
{

}

int System::Register(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "getPowerInfo",		GetPowerInfo 		},
		{ "getProcessorCount",	GetProcessorCount	},
		{ "getOS",				GetOS				},
		{ "getLanguage",		GetLanguage			},
		{ "getWifiStrength",	GetWifiStrength		},
		{ "hasWifiConnection",	HasWifiConnection	},
		{ "getRegion",			GetRegion			},
		{ "getUsername",		GetUsername			},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);

	return 1;
}