#pragma once

#include <common/luax.hpp>

#include <objects/joint/types/frictionjoint/frictionjoint.hpp>
#include <objects/joint/wrap_joint.hpp>

namespace Wrap_FrictionJoint
{
    love::FrictionJoint* CheckFrictionJoint(lua_State* L, int index);

    int SetMaxForce(lua_State* L);

    int GetMaxForce(lua_State* L);

    int SetMaxTorque(lua_State* L);

    int GetMaxTorque(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_FrictionJoint
