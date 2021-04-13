#pragma once

#include "objects/texture/wrap_texture.h"
#include "objects/canvas/canvas.h"

#include "common/luax.h"

namespace Wrap_Canvas
{
    int RenderTo(lua_State * L);

    love::Canvas * CheckCanvas(lua_State * L, int index);

    int Register(lua_State * L);
}
