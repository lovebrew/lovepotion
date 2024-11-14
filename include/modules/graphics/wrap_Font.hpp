#pragma once

#include "common/luax.hpp"
#include "modules/graphics/Font.tcc"

namespace love
{
    FontBase* luax_checkfont(lua_State* L, int index);

    void luax_checkcoloredstring(lua_State* L, int index, std::vector<ColoredString>& strings);

    int open_font(lua_State* L);
} // namespace love

namespace Wrap_Font
{
    int getWidth(lua_State* L);

    int getHeight(lua_State* L);

    int getWrap(lua_State* L);

    int setLineHeight(lua_State* L);

    int getLineHeight(lua_State* L);

    int setFilter(lua_State* L);

    int getFilter(lua_State* L);

    int getAscent(lua_State* L);

    int getDescent(lua_State* L);

    int getBaseline(lua_State* L);

    int hasGlyphs(lua_State* L);

    int getKerning(lua_State* L);

    int setFallbacks(lua_State* L);

    int getDPIScale(lua_State* L);
} // namespace Wrap_Font
