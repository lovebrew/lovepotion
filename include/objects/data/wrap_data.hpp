#pragma once

#include <common/data.hpp>
#include <common/luax.hpp>

namespace Wrap_Data
{
    love::Data* CheckData(lua_State* L, int index);

    int GetPointer(lua_State* L);

    int GetSize(lua_State* L);

    int GetString(lua_State* L);

    extern const luaL_Reg functions[0x0D];

    int Register(lua_State* L);
} // namespace Wrap_Data
