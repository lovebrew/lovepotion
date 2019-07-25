#include "common/runtime.h"
#include "modules/timer.h"

//Löve2D Functions

//love.timer.getFPS
int Timer::GetFPS(lua_State * L)
{
    lua_pushnumber(L, round(totalFrames));

    return 1;
}

//love.timer.getTime
int Timer::GetTime(lua_State * L)
{
    u32 time = SDL_GetTicks();
    lua_pushnumber(L, time * 0.001f);

    return 1;
}

//love.timer.getDelta
int Timer::GetDelta(lua_State * L)
{
    lua_pushnumber(L, deltatime);

    return 1;
}

//love.timer.step
int Timer::Step(lua_State * L)
{
    lastTime = currentTime;

    currentTime = SDL_GetTicks();

    deltatime = (currentTime - lastTime) * 0.001f;

    if (deltatime < 0)
        deltatime = 0;

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

float Timer::GetDelta()
{
    return deltatime;
}

void Timer::Tick()
{
    frames++;
    float delta = SDL_GetTicks() - lastCountTime;

    if (delta >= 1000.0f)
    {
        totalFrames = (frames / delta) * 1000.0f;
        frames = 0;
        lastCountTime = SDL_GetTicks();
    }
}

int Timer::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "getDelta", GetDelta },
        { "getFPS",   GetFPS   },
        { "getTime",  GetTime  },
        { "sleep",    Sleep    },
        { "step",     Step     },
        { 0, 0 }
    };

    luaL_newlib(L, reg);

    return 1;
}
