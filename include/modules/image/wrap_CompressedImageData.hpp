#pragma once

#include "common/luax.hpp"
#include "modules/image/CompressedImageData.hpp"

namespace love
{
    CompressedImageData* luax_checkcompressedimagedata(lua_State* L, int index);

    int open_compressedimagedata(lua_State* L);
} // namespace love

namespace Wrap_CompressedImageData
{
    int clone(lua_State* L);

    int getWidth(lua_State* L);

    int getHeight(lua_State* L);

    int getDimensions(lua_State* L);

    int getMipmapCount(lua_State* L);

    int getFormat(lua_State* L);

    int setLinear(lua_State* L);

    int isLinear(lua_State* L);
} // namespace Wrap_CompressedImageData
