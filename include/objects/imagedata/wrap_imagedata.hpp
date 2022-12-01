#pragma once

#include <common/luax.hpp>
#include <objects/imagedata_ext.hpp>

namespace Wrap_ImageData
{
    love::ImageData<love::Console::Which>* CheckImageData(lua_State* L, int index);

    int Clone(lua_State* L);

    int GetFormat(lua_State* L);

    int GetWidth(lua_State* L);

    int GetHeight(lua_State* L);

    int GetDimensions(lua_State* L);

    int GetPixel(lua_State* L);

    int SetPixel(lua_State* L);

    int __MapPixelUnsafe(lua_State* L);

    int Paste(lua_State* L);

    int Encode(lua_State* L);

    int __PerformAtomic(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_ImageData
