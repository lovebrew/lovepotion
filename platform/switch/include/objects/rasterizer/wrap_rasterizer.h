#pragma once

#include "freetype/rasterizer.h"
#include "common/luax.h"

namespace Wrap_Rasterizer
{
    int GetHeight(lua_State* L);

    int GetAdvance(lua_State* L);

    int GetAscent(lua_State* L);

    int GetDescent(lua_State* L);

    int GetLineHeight(lua_State* L);

    int GetGlyphData(lua_State* L);

    int GetGlyphCount(lua_State* L);

    int HasGlyphs(lua_State* L);

    love::Rasterizer* CheckRasterizer(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_Rasterizer
