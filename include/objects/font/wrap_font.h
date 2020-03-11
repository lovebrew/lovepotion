#pragma once

#include "common/mmath.h"
#include "objects/font/font.h"

namespace Wrap_Font
{
    int GetWidth(lua_State * L);

    int GetHeight(lua_State * L);

    love::Font * CheckFont(lua_State * L, int index);

    void CheckColoredString(lua_State * L, int index, std::vector<love::Font::ColoredString> & strings);

    int Register(lua_State * L);
}
