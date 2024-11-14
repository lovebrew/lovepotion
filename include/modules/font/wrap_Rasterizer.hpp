#pragma once

#include "common/luax.hpp"
#include "modules/font/Rasterizer.hpp"

namespace love
{
    Rasterizer* luax_checkrasterizer(lua_State* L, int index);

    int open_rasterizer(lua_State* L);
} // namespace love

namespace Wrap_Rasterizer
{
    int getHeight(lua_State* L);

    int getAdvance(lua_State* L);

    int getAscent(lua_State* L);

    int getDescent(lua_State* L);

    int getLineHeight(lua_State* L);

    int getGlyphData(lua_State* L);

    int getGlyphCount(lua_State* L);

    int hasGlyphs(lua_State* L);
}
