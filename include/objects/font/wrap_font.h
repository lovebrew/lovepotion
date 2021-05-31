#pragma once

#include "common/luax.h"
#include "objects/font/font.h"

namespace Wrap_Font
{
    int GetWidth(lua_State* L);

    int GetHeight(lua_State* L);

    int GetWrap(lua_State* L);

    int SetLineHeight(lua_State* L);

    int GetLineHeight(lua_State* L);

    int SetFilter(lua_State* L);

    int GetFilter(lua_State* L);

    int GetAscent(lua_State* L);

    int GetDescent(lua_State* L);

    int GetBaseline(lua_State* L);

    int HasGlyphs(lua_State* L);

    int GetKerning(lua_State* L);

    int SetFallbacks(lua_State* L);

    int GetDPIScale(lua_State* L);

    love::Font* CheckFont(lua_State* L, int index);

    void CheckColoredString(lua_State* L, int index,
                            std::vector<love::Font::ColoredString>& strings);

    int Register(lua_State* L);
} // namespace Wrap_Font
