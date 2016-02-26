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

int roundNumber(float num) {
    return num < 0 ? num - 0.5 : num + 0.5;
}

int prevTime = 0;
int currTime = 0;
float dt;

static int timerFPS(lua_State *L) { // love.timer.getFPS()

	lua_pushnumber(L, roundNumber(sf2d_get_fps()));

	return 1;

}

static int timerGetTime(lua_State *L) { // love.timer.getTime()

	int m = osGetTime();

	lua_pushnumber(L, m * 0.001);

	return 1;

}

static int timerStep(lua_State *L) { // love.timer.step()

	prevTime = currTime;

	currTime = osGetTime();

	dt = currTime - prevTime;

	dt = dt * 0.001;

	return 0;

}

static int timerGetDelta() { // love.timer.getDelta()

	if (dt < 0) dt = 0; // Fix nasty timer bug

	lua_pushnumber(L, dt);

	return 1;

}

int initLoveTimer(lua_State *L) {

	luaL_Reg reg[] = {
		{ "getFPS",		timerFPS		},
		{ "getTime",	timerGetTime	},
		{ "step",		timerStep		},
		{ "getDelta",	timerGetDelta	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);

	return 1;

}