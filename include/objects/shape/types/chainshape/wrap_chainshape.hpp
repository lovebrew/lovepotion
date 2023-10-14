#pragma once

#include <common/luax.hpp>

#include <objects/shape/types/chainshape/chainshape.hpp>
#include <objects/shape/wrap_shape.hpp>

namespace Wrap_ChainShape
{
    love::ChainShape* CheckChainShape(lua_State* L, int index);

    int SetNextVertex(lua_State* L);

    int SetPreviousVertex(lua_State* L);

    int GetVertexCount(lua_State* L);

    int GetPoint(lua_State* L);

    int GetNextVertex(lua_State* L);

    int GetPreviousVertex(lua_State* L);

    int GetPoints(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_ChainShape
