#pragma once

#include "common/luax.h"

#include "chainshape/chainshape.h"

namespace Wrap_ChainShape
{
    int SetNextVertex(lua_State* L);

    int SetPreviousVertex(lua_State* L);

    int GetChildEdge(lua_State* L);

    int GetVertexCount(lua_State* L);

    int GetPoint(lua_State* L);

    int GetNextVertex(lua_State* L);

    int GetPreviousVertex(lua_State* L);

    int GetPoints(lua_State* L);

    love::ChainShape* CheckChainShape(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_ChainShape
