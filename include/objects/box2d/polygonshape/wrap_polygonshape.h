#pragma once

#include "common/luax.h"
#include "polygonshape/polygonshape.h"

namespace Wrap_PolygonShape
{
    love::PolygonShape* CheckPolygonShape(lua_State* L, int index);

    int GetPoints(lua_State* L);

    int Validate(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_PolygonShape
