#include "runtime.h"
#include "system.h"

void System::Initialize()
{
	cfguInit();
	ptmuInit();
	acInit();
	mcuHwcInit();

	osSetSpeedupEnable(true);
}

//Löve2D Functions

//love.system.getOS
int System::GetOS(lua_State * L)
{
	lua_pushstring(L, "Horizon");

	return 1;
}

//love.system.getProcessorCount
int System::GetProcessorCount(lua_State * L)
{
	u8 model;
	CFGU_GetSystemModel(&model);

	int processorCount = 2;
	if (model == 2 || model == 4)
		processorCount = 4;

	lua_pushnumber(L, processorCount);

	return 1;
}

//love.system.getPowerInfo
int System::GetPowerInfo(lua_State * L)
{
	u8 batteryPercent = 100;

	MCUHWC_GetBatteryLevel(&batteryPercent);
	
	u8 batteryStateBool;
	PTMU_GetBatteryChargeState(&batteryStateBool);
	
	std::string batteryState = "battery";
	if (batteryStateBool == 1)
		batteryState = "charging";
	else if (batteryStateBool == 1 && batteryPercent == 100)
		batteryState = "charged";

	lua_pushstring(L, batteryState.c_str());
	lua_pushnumber(L, batteryPercent);
	lua_pushnil(L);
	
	return 3;
}

//love.system.getRegion
int System::GetRegion(lua_State * L)
{
	u8 region;
	CFGU_SecureInfoGetRegion(&region);

	const char * regionName;
	switch (region)
	{
		case CFG_REGION_USA:
			regionName = "USA";
			break;
		case CFG_REGION_EUR:
			regionName = "Europe";
			break;
		case CFG_REGION_AUS:
			regionName = "Australia";
			break;
		case CFG_REGION_CHN:
			regionName = "China";
			break;
		case CFG_REGION_KOR:
			regionName = "Korea";
			break;
		case CFG_REGION_TWN:
			regionName = "Taiwan";
			break;
		default:
			regionName = "Japan";
			break;
	}

	lua_pushstring(L, regionName);

	return 1;
}

//love.system.getModel
int System::GetModel(lua_State * L)
{
	u8 model;
	CFGU_GetSystemModel(&model);

	const char * modelName;
	switch (model)
	{
		case 1:
			modelName = "O3DSXL";
			break;
		case 2:
			modelName = "N3DS";
			break;
		case 3:
			modelName = "2DS";
			break;
		case 4:
			modelName = "N3DSXL";
			break;
		default:
			modelName = "O3DS";
			break;
	}

	lua_pushstring(L, modelName);

	return 1;
}

//love.system.getLanguage
int System::GetLanguage(lua_State * L)
{
	u8 language;
	CFGU_GetSystemLanguage(&language);
	
	const char * languageName;
	switch (language)
	{
		case CFG_LANGUAGE_EN:
			languageName = "English";
			break;
		case CFG_LANGUAGE_FR:
			languageName = "French";
			break;
		case CFG_LANGUAGE_DE:
			languageName = "German";
			break;
		case CFG_LANGUAGE_IT:	
			languageName = "Italian";
			break;
		case CFG_LANGUAGE_ES:
			languageName = "Spanish";
			break;
		case CFG_LANGUAGE_ZH:
			languageName = "Simplified Chinese";
			break;
		case CFG_LANGUAGE_KO:
			languageName = "Korean";
			break;
		case CFG_LANGUAGE_NL:
			languageName = "Dutch";
			break;
		case CFG_LANGUAGE_PT:
			languageName = "Portugese";
			break;
		case CFG_LANGUAGE_RU:
			languageName = "Russian";
			break;
		case CFG_LANGUAGE_TW:
			languageName = "Traditional Chinese";
			break;
		default:
			languageName = "Japanese";
			break;
	}

	lua_pushstring(L, languageName);
	
	return 1;
}

//love.system.getWifiStrength
int System::GetWifiStrength(lua_State * L)
{
	u8 wifiStrength = osGetWifiStrength();

	lua_pushnumber(L, wifiStrength);

	return 1;
}

//love.system.hasWifiConnection
int System::HasWifiConnection(lua_State * L)
{
	u32 status;
	ACU_GetWifiStatus(&status);

	lua_pushboolean(L, status != 0);

	return 1;
}

//love.system.getLinearMemory
int System::GetLinearMemory(lua_State * L)
{
	u32 linearMemory;

	linearMemory = linearSpaceFree();
	
	lua_pushnumber(L, linearMemory);
	
	return 1;
}

//love.system.getUsername
int System::GetUsername(lua_State * L)
{
	u16 utf16_username[0x1C] = {0};

	CFGU_GetConfigInfoBlk2(0x1C, 0x000A0000, (u8 *)utf16_username);
	
	ssize_t utf8_len = utf16_to_utf8(NULL, utf16_username, 0);

	string username = {utf8_len, '\0'};

	utf16_to_utf8((uint8_t *)username.data(), utf16_username, utf8_len);

	username[utf8_len] = '\0';

	lua_pushstring(L, username.c_str());

	return 1;
}

//End Löve2D Functions

void System::Exit()
{
	ptmuExit();
	cfguExit();
	acExit();
	mcuHwcExit();
}

int System::Register(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "getPowerInfo",		GetPowerInfo 		},
		{ "getProcessorCount",	GetProcessorCount	},
		{ "getOS",				GetOS				},
		{ "getLanguage",		GetLanguage			},
		{ "getModel",			GetModel 			},
		{ "getWifiStrength",	GetWifiStrength		},
		{ "hasWifiConnection",	HasWifiConnection	},
		{ "getRegion",			GetRegion			},
		{ "getLinearMemory",	GetLinearMemory		},
		{ "getUsername",		GetUsername			},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);

	return 1;
}