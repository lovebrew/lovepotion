#pragma once

#include "common/luax.h"
#include "objects/thread/luathread.h"

namespace Wrap_LuaThread
{
    int Start(lua_State* L);

    int Wait(lua_State* L);

    int GetError(lua_State* L);

    int IsRunning(lua_State* L);

    love::LuaThread* CheckThread(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_LuaThread
