#pragma once

#include "common/luax.hpp"

namespace Wrap_FontModule
{
    int newRasterizer(lua_State* L);

    int newTrueTypeRasterizer(lua_State* L);

    int newBMFontRasterizer(lua_State* L);

    int newImageRasterizer(lua_State* L);

    int newGlyphData(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_FontModule
