#pragma once

#include <common/luax.hpp>
#include <objects/font/font.hpp>

namespace Wrap_Font
{
    love::Font* CheckFont(lua_State* L, int index);

    void CheckColoredString(lua_State* L, int index, love::Font::ColoredStrings& strings);

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

    extern std::span<const luaL_Reg> extensions;
} // namespace Wrap_Font
