#pragma once

#include "circleshape/circleshape.h"
#include "common/luax.h"

namespace Wrap_CircleShape
{
    int GetRadius(lua_State* L);

    int GetPoint(lua_State* L);

    int SetPoint(lua_State* L);

    int SetRadius(lua_State* L);

    love::CircleShape* CheckCircleShape(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_CircleShape
