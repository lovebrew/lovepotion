#pragma once

#include "objects/texture/wrap_texture.h"
#include "objects/canvas/canvas.h"

namespace Wrap_Canvas
{
    love::Canvas * CheckCanvas(lua_State * L, int index);

    int Register(lua_State * L);
}