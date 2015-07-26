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
#include <time.h>

int prevTime = 0;
int currTime = 0;
long double dt;

static int timerFPS(lua_State *L) { // love.timer.getFPS()

	lua_pushnumber(L, sf2d_get_fps());

	return 1;

}

static int timerGetTime(lua_State *L) { // love.timer.getTime()

	int m = osGetTime();

	lua_pushnumber(L, m);

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

	lua_pushnumber(L, dt);

	return 1;

}

int initLoveTimer(lua_State *L) {

	registerFunction("timer", "getFPS", timerFPS);
	registerFunction("timer", "getTime", timerGetTime);
	registerFunction("timer", "step", timerStep);
	registerFunction("timer", "getDelta", timerGetDelta);

	return 1;

}