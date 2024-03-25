#pragma once

#include "common/luax.hpp"
#include "modules/data/ByteData.hpp"

namespace love
{
    ByteData* luax_checkbytedata(lua_State* L, int index);

    int open_bytedata(lua_State* L);
} // namespace love

namespace Wrap_ByteData
{
    int clone(lua_State* L);

    int setString(lua_State* L);
} // namespace Wrap_ByteData
