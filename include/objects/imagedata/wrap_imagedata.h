#pragma once

#include "common/luax.h"
#include "objects/imagedata/imagedata.h"

namespace Wrap_ImageData
{
    int Clone(lua_State* L);

    int GetFormat(lua_State* L);

    int GetWidth(lua_State* L);

    int GetHeight(lua_State* L);

    int GetDimensions(lua_State* L);

    int GetPixel(lua_State* L);

    int SetPixel(lua_State* L);

    int _MapPixelUnsafe(lua_State* L);

    int Paste(lua_State* L);

    int Encode(lua_State* L);

    int _PerformAtomic(lua_State* L);

    love::ImageData* CheckImageData(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_ImageData
