#include "common/runtime.h"
#include "system.h"

love::System * love::System::instance = 0;

using love::System;

int System::GetOS(lua_State * L)
{
	lua_pushstring(L, "Horizon");

	return 1;
}

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

int System::GetPowerInfo(lua_State * L)
{
	u8 batteryPercent;
	
	PTMU_GetBatteryLevel(&batteryPercent);
	
	u8 batteryStateBool;
	PTMU_GetBatteryChargeState(&batteryStateBool);
	
	const char * batteryState = "battery";
	if (batteryStateBool == 1)
		batteryState = "charging";
	
	lua_pushstring(L, batteryState);
	lua_pushnumber(L, batteryPercent * 20); //PTMU_GetBatteryLevel returns a number between 0 and 5, so I multiply it for 20 to match LÖVE, which gives 0-100
	lua_pushnil(L);
	
	return 3;
}

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

int System::GetSystemLanguage(lua_State * L)
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

int System::GetWifiStrength(lua_State * L)
{
	u8 wifiStrength = osGetWifiStrength();

	lua_pushnumber(L, wifiStrength);

	return 1;
}

int System::GetWifiStatus(lua_State * L)
{
	u32 status;
	ACU_GetWifiStatus(&status);

	lua_pushboolean(L, status != 0);

	return 1;
}

int System::GetLinearMemory(lua_State * L)
{
	u32 linearMemory;

	linearMemory = linearSpaceFree();
	
	lua_pushnumber(L, linearMemory);
	
	return 1;
}

int System::SetConsole(lua_State * L)
{
	if (!console->IsEnabled())
		console->Enable(GFX_BOTTOM); //default?
	else
		console->Disable();
}

void systemExit()
{
	ptmuExit();
	cfguExit();
	acExit();
}

int systemInit(lua_State * L)
{
	cfguInit();
	ptmuInit();
	acInit();

	luaL_Reg reg[] = 
	{
		{ "getLanguage",		love::System::GetSystemLanguage },
		{ "getPowerInfo",		love::System::GetPowerInfo 		},
		{ "getProcessorCount",	love::System::GetProcessorCount },
		{ "getModel",			love::System::GetModel 			},
		{ "getWifiStrength",	love::System::GetWifiStrength	},
		{ "getWifiStatus",		love::System::GetWifiStatus		},
		{ "getOS",				love::System::GetOS				},
		{ "getRegion",			love::System::GetRegion			},
		{ "getLinearMemory",	love::System::GetLinearMemory	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}