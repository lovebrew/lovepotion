#pragma once

#include "common/luax.hpp"
#include "modules/image/Image.hpp"

namespace Wrap_Image
{
    int newImageData(lua_State* L);

    int newCompressedData(lua_State* L);

    int isCompressed(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_Image
