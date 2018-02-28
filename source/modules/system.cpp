#include "common/runtime.h"
#include "modules/system.h"

void System::Initialize()
{
	/*
	** Include things like configuration
	** Account name that runs the game
	** Battery info (power, state)
	** Region and language
	*/
	setInitialize();
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
	s32 region;
	setGetRegionCode(&region);

	string value;

	return 0;
}

//love.system.getLanguage
int System::GetLanguage(lua_State * L)
{
	u64 language;
	setGetSystemLanguage(&language);

	string value;
	switch (language)
	{
		case SetLanguage_ENUS:
			value = "American English";
			break;
		case SetLanguage_FR:
			value = "French";
			break;
		case SetLanguage_DE:
			value = "German";
			break;
		case SetLanguage_IT:
			value = "Italian";
			break;
		case SetLanguage_ES:
			value = "Spanish";
			break;
		case SetLanguage_ZHCN:
			value = "Simplified Chinese";
			break;
		case SetLanguage_KO:
			value = "Korean";
			break;
		case SetLanguage_NL:
			value = "Dutch";
			break;
		case SetLanguage_PT:
			value = "Portuguese";
			break;
		case SetLanguage_RU:
			value = "Russian";
			break;
		case SetLanguage_ZHTW:
			value = "Traditional Chinese";
			break;
		case SetLanguage_ENGB:
			value = "British English";
			break;
		case SetLanguage_FRCA:
			value = "Canadian French";
			break;
		case SetLanguage_ES419:
			value = "Latin American Spanish";
			break;
		default:
			value = "Japanese";
			break;
	}

	lua_pushstring(L, value.c_str());

	return 1;
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
	setExit();
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