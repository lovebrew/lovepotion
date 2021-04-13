#pragma once

#include "common/luax.h"
#include "modules/timer/timer.h"
namespace Wrap_Timer
{
    int GetAverageDelta(lua_State* L);

    int GetDelta(lua_State* L);

    int GetFPS(lua_State* L);

    int GetTime(lua_State* L);

    int Sleep(lua_State* L);

    int Step(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Timer
