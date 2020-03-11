#pragma once

#include "common/data.h"

namespace Wrap_Data
{
    int GetPointer(lua_State * L);

    int GetSize(lua_State * L);

    int GetString(lua_State * L);

    love::Data * CheckData(lua_State * L, int index);

    extern luaL_Reg functions[4];

    int Register(lua_State * L);
}
