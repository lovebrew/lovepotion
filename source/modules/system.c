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

#include "../shared.h"

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

	luaL_Reg reg[] = {
		{ "getOS",			systemGetOS			},
		{ "getPowerInfo",	systemGetPowerInfo	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);

	return 1;

}