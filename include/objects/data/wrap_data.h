#pragma once

#include "common/data.h"
#include "common/luax.h"

namespace Wrap_Data
{
    int GetPointer(lua_State* L);

    int GetSize(lua_State* L);

    int GetString(lua_State* L);

    love::Data* CheckData(lua_State* L, int index);

    extern const luaL_Reg functions[4];

    int Register(lua_State* L);
} // namespace Wrap_Data
