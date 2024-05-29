#pragma once

#include "common/luax.hpp"
#include "modules/graphics/Font.tcc"

namespace love
{
    FontBase* luax_checkfont(lua_State* L, int index);

    void luax_checkcoloredstring(lua_State* L, int index, std::vector<ColoredString>& strings);

    int open_font(lua_State* L);
} // namespace love
