#pragma once

#include <common/luax.hpp>

#include <objects/data/bytedata/bytedata.hpp>
#include <objects/data/wrap_data.hpp>

namespace Wrap_ByteData
{
    int Clone(lua_State* L);

    love::ByteData* CheckByteData(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_ByteData
