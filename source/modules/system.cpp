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
	accountInitialize();
	plInitialize();
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

	lua_pushstring(L, REGIONS[region].c_str());

	return 1;
}

//love.system.getLanguage
int System::GetLanguage(lua_State * L)
{
	u64 languageString;
	s32 languageID;

	setGetSystemLanguage(&languageString);
	setMakeLanguage(languageString, &languageID);

	lua_pushstring(L, LANGUAGES[languageID].c_str());

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
	bool accountSelected = 0;
	
	u128 userID = 0;
	AccountProfile profile;
	AccountUserData userdata;
	AccountProfileBase profilebase;

	char username[0x21];

	memset(&userdata, 0, sizeof(userdata));
	memset(&profilebase, 0, sizeof(profilebase));

	Result successCode = accountGetActiveUser(&userID, &accountSelected);

	if (R_SUCCEEDED(successCode))
	{
		successCode = accountGetProfile(&profile, userID);

		if (R_SUCCEEDED(successCode))
		{
			successCode = accountProfileGet(&profile, &userdata, &profilebase);

			if (R_SUCCEEDED(successCode))
			{
				memset(username, 0, sizeof(username));
				strncpy(username, profilebase.username, sizeof(profilebase.username));

				lua_pushstring(L, username);

				accountProfileClose(&profile);

				return 1;
			}
		}
	}

	//In the event it fails, close it
	accountProfileClose(&profile);
	lua_pushnil(L);

	return 1;
}

//End Löve2D Functions

void System::Exit()
{
	setExit();
	accountExit();
	plExit();
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