#pragma once

#include "objects/data/wrap_data.h"
#include "objects/data/compressed/compresseddata.h"

namespace Wrap_CompressedData
{
    love::CompressedData * CheckCompressedData(lua_State * L, int index);

    int Clone(lua_State * L);

    int GetFormat(lua_State * L);

    int Register(lua_State * L);
}
