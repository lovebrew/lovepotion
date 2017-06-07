#include "common/runtime.h"
#include "timer.h"

float love::Timer::dt = 0;
float love::Timer::lastTick = 0;
float love::Timer::currentTick = 0;

using love::Timer;

int Timer::GetTime(lua_State * L)
{
	int m = osGetTime();

	lua_pushnumber(L, m * 0.001);

	return 1;
}

int Timer::GetFPS(lua_State * L)
{
	lua_pushnil(L);

	return 1;
}

int Timer::Step(lua_State * L)
{
	lastTick = currentTick;

	currentTick = osGetTime();

	dt = currentTick - lastTick;

	dt = dt * 0.001;

	return 0;
}

int Timer::GetDelta(lua_State * L)
{
	if (dt < 0)
		dt = 0;

	lua_pushnumber(L, dt);

	return 1;
}

int timerInit(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "getTime",	love::Timer::GetTime	},
		{ "step",		love::Timer::Step		},
		{ "getDelta",	love::Timer::GetDelta	},
		{ "getFPS",		love::Timer::GetFPS		},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}