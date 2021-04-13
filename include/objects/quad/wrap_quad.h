#pragma once

#include "common/luax.h"
#include "objects/quad/quad.h"

namespace Wrap_Quad
{
    int GetTextureDimensions(lua_State* L);

    int GetViewport(lua_State* L);

    int SetViewport(lua_State* L);

    love::Quad* CheckQuad(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_Quad
