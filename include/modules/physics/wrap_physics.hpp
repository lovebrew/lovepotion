#pragma once

#include <common/luax.hpp>
#include <modules/physics/physics.hpp>

namespace Wrap_Physics
{
    int NewWorld(lua_State* L);

    int NewBody(lua_State* L);

    int NewCircleBody(lua_State* L);

    int NewRectangleBody(lua_State* L);

    int NewPolygonBody(lua_State* L);

    int NewEdgeBody(lua_State* L);

    int NewChainBody(lua_State* L);

    int NewCircleShape(lua_State* L);

    int NewRectangleShape(lua_State* L);

    int NewEdgeShape(lua_State* L);

    int NewPolygonShape(lua_State* L);

    int NewChainShape(lua_State* L);

    int GetDistance(lua_State* L);

    int SetMeter(lua_State* L);

    int GetMeter(lua_State* L);

    int ComputeLinearStiffness(lua_State* L);

    int ComputeLinearFrequency(lua_State* L);

    int ComputeAngularStiffness(lua_State* L);

    int ComputeAngularFrequency(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Physics
