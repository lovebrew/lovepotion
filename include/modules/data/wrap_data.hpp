#pragma once

#include "common/Data.hpp"
#include "common/luax.hpp"

namespace love
{
    Data* luax_checkdata(lua_State* L, int idx);

    int open_data(lua_State* L);
} // namespace love

namespace Wrap_Data
{
    int getString(lua_State* L);

    int getPointer(lua_State* L);

    int getFFIPointer(lua_State* L);

    int getSize(lua_State* L);

    int performAtomic(lua_State* L);

    extern luaL_Reg functions[13];
} // namespace Wrap_Data
