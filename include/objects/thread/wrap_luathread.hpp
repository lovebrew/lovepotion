#pragma once

#include <common/luax.hpp>
#include <objects/thread/luathread.hpp>

namespace Wrap_LuaThread
{
    int Start(lua_State* L);

    int Wait(lua_State* L);

    int GetError(lua_State* L);

    int IsRunning(lua_State* L);

    love::LuaThread* CheckThread(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_LuaThread
