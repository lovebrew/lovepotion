#pragma once

#include "objects/data/wrap_data.h"
#include "objects/data/byte/bytedata.h"

namespace Wrap_ByteData
{
    love::ByteData * CheckByteData(lua_State * L, int index);

    int Register(lua_State * L);
}
