#pragma once

#include "common/luax.hpp"

namespace Wrap_Timer
{
    int step(lua_State* L);

    int getDelta(lua_State* L);

    int getFPS(lua_State* L);

    int getAverageDelta(lua_State* L);

    int sleep(lua_State* L);

    int getTime(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_Timer
