#pragma once

#include <common/luax.hpp>

#include <objects/joint/types/mousejoint/mousejoint.hpp>
#include <objects/joint/wrap_joint.hpp>

namespace Wrap_MouseJoint
{
    love::MouseJoint* CheckMouseJoint(lua_State* L, int index);

    int SetTarget(lua_State* L);

    int GetTarget(lua_State* L);

    int SetMaxForce(lua_State* L);

    int GetMaxForce(lua_State* L);

    int SetStiffness(lua_State* L);

    int GetStiffness(lua_State* L);

    int SetDamping(lua_State* L);

    int GetDamping(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_MouseJoint
