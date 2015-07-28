#include "../include/lua/lua.h"
#include "../include/lua/lualib.h"
#include "../include/lua/lauxlib.h"

#include "../shared.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <3ds.h>
#include <sf2d.h>

u8 buf0[0x300];
u8 buf1[0x20]; // APT_DoAppJump() Buffers.

static int systemOpenURL() { // love.system.openURL() -- Might not work on all systems.

	// Clear buffers
	memset(buf0, 0, 0x300);
	memset(buf1, 0, 0x20);

	aptOpenSession();

	APT_PrepareToDoAppJump(NULL, 0, 0x20009D020, 0); // Browser App ID

	APT_DoAppJump(NULL, 0x300, 0x20, buf0, buf1);

	aptCloseSession();

	return 0;

}

static int systemGetOS(lua_State *L) { // love.system.getOS()

	lua_pushstring(L, "3ds");
	return 1;

}

static int systemGetPowerInfo(lua_State *L) { // love.system.getPowerInfo() TODO: Test this.

	u8 batteryStateBool;
	PTMU_GetBatteryChargeState(NULL, &batteryStateBool);

	const char *batteryState;

	if (batteryStateBool == 0) {

		batteryState = "battery";

	} else if (batteryStateBool == 1) {

		batteryState = "charging";

	}

	u8 batteryPercent;

	PTMU_GetBatteryLevel(NULL, &batteryPercent);

	lua_pushstring(L, batteryState);
	lua_pushnumber(L, batteryPercent);
	lua_pushnil(L);

	return 3;

}

int initLoveSystem(lua_State *L) {

	registerFunction("system", "openURL", systemOpenURL);
	registerFunction("system", "getOS", systemGetOS);
	registerFunction("system", "getPowerInfo", systemGetPowerInfo);

	return 1;

}