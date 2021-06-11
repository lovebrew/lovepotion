#pragma once

#include "common/luax.h"
#include "edgeshape/edgeshape.h"

namespace Wrap_EdgeShape
{
    int SetNextVertex(lua_State* L);

    int SetPreviousVertex(lua_State* L);

    int GetNextVertex(lua_State* L);

    int GetPreviousVertex(lua_State* L);

    int GetPoints(lua_State* L);

    love::EdgeShape* CheckEdgeShape(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_EdgeShape
