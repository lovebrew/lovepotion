#pragma once

#include "common/luax.hpp"
#include "modules/data/wrap_Data.hpp"
#include "modules/font/GlyphData.hpp"

namespace love
{
    GlyphData* luax_checkglyphdata(lua_State* L, int index);

    int open_glyphdata(lua_State* L);
} // namespace love

namespace Wrap_GlyphData
{
    int clone(lua_State* L);

    int getWidth(lua_State* L);

    int getHeight(lua_State* L);

    int getDimensions(lua_State* L);

    int getGlyph(lua_State* L);

    int getGlyphString(lua_State* L);

    int getAdvance(lua_State* L);

    int getBearing(lua_State* L);

    int getBoundingBox(lua_State* L);

    int getFormat(lua_State* L);
} // namespace Wrap_GlyphData
