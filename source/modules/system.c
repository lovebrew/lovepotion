// This code is licensed under the MIT Open Source License.

// Copyright (c) 2015 Ruairidh Carmichael - ruairidhcarmichael@live.co.uk

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <shared.h>

static int systemGetOS(lua_State *L) { // love.system.getOS()

	lua_pushstring(L, "3ds");
	return 1;

}

static int systemGetModel(lua_State *L) { // love.system.getModel()
	
	u8 model;
	CFGU_GetSystemModel(&model);
	
	if (model == 0) { // Original 3DS
		
		lua_pushstring(L, "o3ds");
		
	} else if (model == 1) { // 3DS XL
		
		lua_pushstring(L, "o3dsxl");
		
	} else if (model == 2) { // New 3DS
		
		lua_pushstring(L, "n3ds");
		
	} else if (model == 3) { // 2DS
		
		lua_pushstring(L, "2ds");
		
	} else if (model == 4) { // New 3DS XL
		
		lua_pushstring(L, "n3dsxl");
		
	}
	
	return 1;

}

static int systemGetLanguage(lua_State *L) { // love.system.getLanguage()
	
	u8 language;
	CFGU_GetSystemLanguage(&language);
	
	if (language == CFG_LANGUAGE_JP) {  // Japanese
		
		lua_pushstring(L, "japanese");
		
	} else if (language == CFG_LANGUAGE_EN) { // English
		
		lua_pushstring(L, "english");
		
	} else if (language == CFG_LANGUAGE_FR) { // French
		
		lua_pushstring(L, "french");
		
	} else if (language == CFG_LANGUAGE_DE) { // German
		
		lua_pushstring(L, "german");
		
	} else if (language == CFG_LANGUAGE_IT) { // Italian
		
		lua_pushstring(L, "italian");
		
	} else if (language == CFG_LANGUAGE_ES) { // Spanish
		
		lua_pushstring(L, "spanish");
		
	} else if (language == CFG_LANGUAGE_ZH) { // Simplified Chinese
		
		lua_pushstring(L, "simplifiedchinese");
		
	} else if (language == CFG_LANGUAGE_KO) { // Korean
		
		lua_pushstring(L, "korean");
		
	} else if (language == CFG_LANGUAGE_NL) { // Dutch
		
		lua_pushstring(L, "dutch");
		
	} else if (language == CFG_LANGUAGE_PT) { // Portugese
		
		lua_pushstring(L, "portugese");
		
	} else if (language == CFG_LANGUAGE_RU) { // Russian
		
		lua_pushstring(L, "russian");
		
	} else if (language == CFG_LANGUAGE_TW) { // Traditional Chinese
		
		lua_pushstring(L, "traiditionalchinese");
		
	}
	
	return 1;

}

static int systemGetRegion(lua_State *L) { // love.system.getRegion()
	
	u8 region;
	CFGU_SecureInfoGetRegion(&region);
	
	if (region == CFG_REGION_JPN) { // Japan
		
		lua_pushstring(L, "japan");
		
	} else if (region == CFG_REGION_USA) { // USA
		
		lua_pushstring(L, "usa");
		
	} else if (region == CFG_REGION_EUR) { // Europe
		
		lua_pushstring(L, "europe");
		
	} else if (region == CFG_REGION_AUS) { // Australia
		
		lua_pushstring(L, "australia");
		
	} else if (region == CFG_REGION_CHN) { // China
		
		lua_pushstring(L, "china");
		
	} else if (region == CFG_REGION_KOR) { // Korea
		
		lua_pushstring(L, "korea");
		
	} else if (region == CFG_REGION_TWN) { // Taiwan
		
		lua_pushstring(L, "taiwan");
		
	}
	
	return 1;

}

static int systemGetPowerInfo(lua_State *L){
	
	u8 batteryPercent;
	
	PTMU_GetBatteryLevel(&batteryPercent);
	
	u8 batteryStateBool;
	PTMU_GetBatteryChargeState(&batteryStateBool);
	
	if (batteryStateBool == 0) {
		
		lua_pushstring(L, "battery");
		
	} else if (batteryStateBool == 1) {
		
		lua_pushstring(L, "charging");
	
	}
	
	lua_pushnumber(L, batteryPercent * 20); //PTMU_GetBatteryLevel returns a number between 0 and 5, so I multiply it for 20 to match LÖVE, which gives 0-100
	lua_pushnil(L);
	
	return 3;
	
}

int initLoveSystem(lua_State *L) {

	luaL_Reg reg[] = {
		{ "getOS",		   systemGetOS		 },
		{ "getPowerInfo",  systemGetPowerInfo},
		{ "getModel",	   systemGetModel    },
		{ "getLanguage",   systemGetLanguage },
		{ "getRegion",     systemGetRegion   },
		{ 0, 0 },
	};

	luaL_newlib(L, reg);

	return 1;

}