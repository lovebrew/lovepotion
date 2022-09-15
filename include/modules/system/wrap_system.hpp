#pragma once

#include <common/luax.hpp>

namespace Wrap_System
{
    int GetOS(lua_State* L);

    int GetProcessorCount(lua_State* L);

    int GetPowerInfo(lua_State* L);

    int GetNetworkInfo(lua_State* L);

    int GetPreferredLocales(lua_State* L);

    int GetModel(lua_State* L);

    int GetUsername(lua_State* L);

    int GetVersion(lua_State* L);

    int GetFriendInfo(lua_State* L);

    int GetSystemTheme(lua_State* L);

    int GetPlayCoins(lua_State* L);

    int SetPlayCoins(lua_State* L);

    extern std::span<const luaL_Reg> extensions;

    int Register(lua_State* L);
} // namespace Wrap_System
