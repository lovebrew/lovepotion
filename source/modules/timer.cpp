#include "runtime.h"
#include "timer.h"

//Deltatime
int prevTime = 0;
int currTime = 0;
float dt = 0;

//Frames per second
float frames = 0;
float fps = 0;
u64 lastCountTime = 0;

//Löve2D Functions

//love.timer.getFPS
int Timer::GetFPS(lua_State * L)
{
	lua_pushnumber(L, fps);

	return 1;
}

//love.timer.getTime
int Timer::GetTime(lua_State * L)
{
	int osTime = svcGetSystemTick();

	lua_pushnumber(L, osTime * 0.001f);

	return 1;
}

//love.timer.getDelta
int Timer::GetDelta(lua_State * L)
{
	lua_pushnumber(L, dt);
	
	return 1;
}

//love.timer.step
int Timer::Step(lua_State * L)
{
	prevTime = currTime;

	currTime = svcGetSystemTick();

	dt = currTime - prevTime;

	dt = dt * 0.001;

	if (dt < 0) 
		dt = 0;

	return 0;
}

//love.timer.sleep
int Timer::Sleep(lua_State * L)
{
	float duration = luaL_checknumber(L, 1);
	u32 sleepDuration = (duration * 1000.0f);

	svcSleepThread(1000000ULL * sleepDuration);

	return 0;
}

//End Löve2D Functions

void Timer::Tick()
{
	frames++;
	u64 delta = svcGetSystemTick() - lastCountTime;
	if (delta >= 1000) {
		fps = frames/(delta/1000.0f);
		frames = 0;
		lastCountTime = svcGetSystemTick();
	}
}

int Timer::Register(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "getFPS",			GetFPS			},
		{ "getDelta",		GetDelta		},
		{ "getTime",		GetTime			},
		{ "sleep",			Sleep			},
		{ "step",			Step			},
		{ 0, 0 }
	};

	luaL_newlib(L, reg);

	return 1;
}