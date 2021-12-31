#pragma once

#include "common/luax.h"
#include "objects/compressedimagedata/compressedimagedata.h"

namespace Wrap_CompressedImageData
{
    int Clone(lua_State* L);

    int GetWidth(lua_State* L);

    int GetHeight(lua_State* L);

    int GetDimensions(lua_State* L);

    int GetMipmapCount(lua_State* L);

    int GetFormat(lua_State* L);

    love::CompressedImageData* CheckCompressedImageData(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_CompressedImageData
