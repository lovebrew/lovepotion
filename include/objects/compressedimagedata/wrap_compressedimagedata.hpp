#pragma once

#include <common/luax.hpp>
#include <objects/compressedimagedata/compressedimagedata.hpp>

namespace Wrap_CompressedImageData
{
    love::CompressedImageData* CheckCompressedImageData(lua_State* L, int index);

    int Clone(lua_State* L);

    int GetWidth(lua_State* L);

    int GetHeight(lua_State* L);

    int GetDimensions(lua_State* L);

    int GetMipmapCount(lua_State* L);

    int GetFormat(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_CompressedImageData