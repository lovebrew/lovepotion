#pragma once

#include "objects/quad/quad.h"
#include "common/luax.h"

namespace Wrap_Quad
{
    int GetTextureDimensions(lua_State * L);

    int GetViewport(lua_State * L);

    int SetViewport(lua_State * L);

    love::Quad * CheckQuad(lua_State * L, int index);

    int Register(lua_State * L);
}
