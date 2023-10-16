#pragma once

#include <common/luax.hpp>

#include <objects/shape/types/circleshape/circleshape.hpp>
#include <objects/shape/wrap_shape.hpp>

namespace Wrap_CircleShape
{
    love::CircleShape* CheckCircleShape(lua_State* L, int index);

    int GetRadius(lua_State* L);

    int SetRadius(lua_State* L);

    int GetPoint(lua_State* L);

    int SetPoint(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_CircleShape
