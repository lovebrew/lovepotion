#pragma once

#include "common/luax.hpp"

namespace Wrap_Event
{
    int pump(lua_State* L);

    int wait(lua_State* L);

    int push(lua_State* L);

    int clear(lua_State* L);

    int quit(lua_State* L);

    int restart(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_Event
