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

    int NewDistanceJoint(lua_State* L);

    int NewFrictionJoint(lua_State* L);

    int NewGearJoint(lua_State* L);

    int NewMotorJoint(lua_State* L);

    int NewMouseJoint(lua_State* L);

    int NewPrismaticJoint(lua_State* L);

    int NewPulleyJoint(lua_State* L);

    int NewRevoluteJoint(lua_State* L);

    int NewRopeJoint(lua_State* L);

    int NewWeldJoint(lua_State* L);

    int NewWheelJoint(lua_State* L);

    int GetDistance(lua_State* L);

    int SetMeter(lua_State* L);

    int GetMeter(lua_State* L);

    int ComputeLinearStiffness(lua_State* L);

    int ComputeLinearFrequency(lua_State* L);

    int ComputeAngularStiffness(lua_State* L);

    int ComputeAngularFrequency(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Physics
