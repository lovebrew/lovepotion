#pragma once

#include "objects/data/wrap_data.h"
#include "freetype/glyphdata.h"

namespace Wrap_GlyphData
{
    int Clone(lua_State * L);

    int GetWidth(lua_State * L);

    int GetHeight(lua_State * L);

    int GetDimensions(lua_State * L);

    int GetGlyph(lua_State * L);

    int GetGlyphString(lua_State * L);

    int GetAdvance(lua_State * L);

    int GetBearing(lua_State * L);

    int GetBoundingBox(lua_State * L);

    love::GlyphData * CheckGlyphData(lua_State * L, int index);

    int Register(lua_State * L);
}