#pragma once

#include "common/luax.hpp"
#include "modules/thread/ThreadModule.hpp"

namespace love
{
    LuaThread* luax_checkthread(lua_State* L, int index);

    int open_thread(lua_State* L);
} // namespace love

namespace Wrap_Thread
{
    int start(lua_State* L);

    int wait(lua_State* L);

    int getError(lua_State* L);

    int isRunning(lua_State* L);
} // namespace Wrap_Thread
