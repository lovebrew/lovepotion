#pragma once

#include "common/luax.hpp"
#include "modules/graphics/Quad.hpp"

namespace love
{
    Quad* luax_checkquad(lua_State* L, int index);

    int open_quad(lua_State* L);
} // namespace love

namespace Wrap_Quad
{
    int setViewport(lua_State* L);

    int getViewport(lua_State* L);

    int getTextureDimensions(lua_State* L);

    int setLayer(lua_State* L);

    int getLayer(lua_State* L);
} // namespace Wrap_Quad
