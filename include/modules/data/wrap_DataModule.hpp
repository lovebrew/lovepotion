#pragma once

#include "common/luax.hpp"
#include "modules/data/DataModule.hpp"

namespace love
{
    data::ContainerType luax_checkcontainertype(lua_State* L, int index);
} // namespace love

namespace Wrap_DataModule
{
    int compress(lua_State* L);

    int decompress(lua_State* L);

    int hash(lua_State* L);

    int encode(lua_State* L);

    int decode(lua_State* L);

    int pack(lua_State* L);

    int unpack(lua_State* L);

    int newByteData(lua_State* L);

    int newDataView(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_DataModule
