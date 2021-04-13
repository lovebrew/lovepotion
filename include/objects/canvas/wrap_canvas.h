#pragma once

#include "objects/canvas/canvas.h"
#include "objects/texture/wrap_texture.h"

#include "common/luax.h"

namespace Wrap_Canvas
{
    int RenderTo(lua_State* L);

    love::Canvas* CheckCanvas(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_Canvas
