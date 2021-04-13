#pragma once

#include "objects/data/compressed/compresseddata.h"
#include "objects/data/wrap_data.h"

#include "common/luax.h"

namespace Wrap_CompressedData
{
    love::CompressedData* CheckCompressedData(lua_State* L, int index);

    int Clone(lua_State* L);

    int GetFormat(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_CompressedData
