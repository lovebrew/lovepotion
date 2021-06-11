#pragma once

#include "common/luax.h"
#include "frictionjoint/frictionjoint.h"

namespace Wrap_FrictionJoint
{
    int SetMaxForce(lua_State* L);

    int GetMaxForce(lua_State* L);

    int SetMaxTorque(lua_State* L);

    int GetMaxTorque(lua_State* L);

    love::FrictionJoint* CheckFrictionJoint(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_FrictionJoint
