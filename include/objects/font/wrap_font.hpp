#pragma once

#include <common/luax.hpp>

#include <objects/font_ext.hpp>

namespace Wrap_Font
{
    love::Font<love::Console::Which>* CheckFont(lua_State* L, int index);

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

    int Register(lua_State* L);

    extern std::function<void(lua_State*)> wrap_extension;
} // namespace Wrap_Font
