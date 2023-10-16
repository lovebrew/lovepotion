#pragma once

#include <common/luax.hpp>
#include <objects/quad/quad.hpp>

namespace Wrap_Quad
{
    love::Quad* CheckQuad(lua_State* L, int index);

    int SetViewport(lua_State* L);

    int GetViewport(lua_State* L);

    int GetTextureDimensions(lua_State* L);

    int SetLayer(lua_State* L);

    int GetLayer(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Quad
