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

static int timerFPS(lua_State *L) { // love.timer.getFPS()

	lua_pushnumber(L, sf2d_get_fps());

	return 1;

}

int initLoveTimer(lua_State *L) {

	registerFunction("timer", "getFPS", timerFPS);

	return 1;

}