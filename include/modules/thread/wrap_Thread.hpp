#pragma once

#include "common/luax.hpp"
#include "modules/thread/ThreadModule.hpp"

namespace Wrap_Thread
{
    int newThread(lua_State* L);

    int getChannel(lua_State* L);

    int newChannel(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_Thread
