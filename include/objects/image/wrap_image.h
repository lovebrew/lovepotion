#pragma once

#include "common/luax.h"

#include "objects/image/image.h"
#include "objects/texture/wrap_texture.h"

namespace Wrap_Image
{
    int GetDimensions(lua_State* L);

    int GetFilter(lua_State* L);

    int GetHeight(lua_State* L);

    int GetWidth(lua_State* L);

    int GetWrap(lua_State* L);

    int SetFilter(lua_State* L);

    int SetWrap(lua_State* L);

    love::Image* CheckImage(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_Image
