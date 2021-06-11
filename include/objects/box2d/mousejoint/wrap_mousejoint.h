#pragma once

#include "common/luax.h"
#include "mousejoint/mousejoint.h"

namespace Wrap_MouseJoint
{
    int SetTarget(lua_State* L);

    int GetTarget(lua_State* L);

    int SetMaxForce(lua_State* L);

    int GetMaxForce(lua_State* L);

    int SetFrequency(lua_State* L);

    int GetFrequency(lua_State* L);

    int SetDampingRatio(lua_State* L);

    int GetDampingRatio(lua_State* L);

    int SetStiffness(lua_State* L);

    int GetStiffness(lua_State* L);

    int SetDamping(lua_State* L);

    int GetDamping(lua_State* L);

    love::MouseJoint* CheckMouseJoint(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_MouseJoint
