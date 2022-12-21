#pragma once

#include <common/luax.hpp>

#include <objects/rasterizer/wrap_rasterizer.hpp>

namespace Wrap_FontModule
{
    int NewRasterizer(lua_State* L);

    int NewGlyphData(lua_State* L);

    int NewTrueTypeRasterizer(lua_State* L);

    int NewBCFNTRasterizer(lua_State* L);

    int Register(lua_State* L);

    extern std::span<const luaL_Reg> extensions;
} // namespace Wrap_FontModule
