#pragma once

#include <common/luax.hpp>

#include <objects/data/compresseddata/compresseddata.hpp>
#include <objects/data/wrap_data.hpp>

namespace Wrap_CompressedData
{
    love::CompressedData* CheckCompressedData(lua_State* L, int index);

    int Clone(lua_State* L);

    int GetFormat(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_CompressedData
