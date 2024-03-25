#pragma once

#include "common/luax.hpp"
#include "modules/data/CompressedData.hpp"

namespace love
{
    CompressedData* luax_checkcompresseddata(lua_State* L, int index);

    int open_compresseddata(lua_State* L);
} // namespace love

namespace Wrap_CompressedData
{
    int clone(lua_State* L);

    int getFormat(lua_State* L);
} // namespace Wrap_CompressedData
