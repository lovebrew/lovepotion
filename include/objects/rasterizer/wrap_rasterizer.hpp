#pragma once

#include <common/luax.hpp>

#include <objects/glyphdata/glyphdata.hpp>
#include <objects/rasterizer_ext.hpp>

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

    love::Rasterizer<love::Console::Which>* CheckRasterizer(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_Rasterizer
