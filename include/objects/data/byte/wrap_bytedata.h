#pragma once

#include "objects/data/byte/bytedata.h"
#include "objects/data/wrap_data.h"

#include "common/luax.h"

namespace Wrap_ByteData
{
    int Clone(lua_State* L);

    love::ByteData* CheckByteData(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_ByteData
