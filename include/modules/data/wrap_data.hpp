#pragma once

#include <common/luax.hpp>
#include <modules/data/data.hpp>

namespace Wrap_DataModule
{
    love::DataModule::ContainerType CheckContainerType(lua_State* L, int index);

    int Hash(lua_State* L);

    int Compress(lua_State* L);

    int Decompress(lua_State* L);

    int Encode(lua_State* L);

    int Decode(lua_State* L);

    int NewByteData(lua_State* L);

    int NewDataView(lua_State* L);

    int Pack(lua_State* L);

    int Unpack(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_DataModule
