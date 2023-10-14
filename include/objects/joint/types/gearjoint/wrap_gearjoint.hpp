#pragma once

#include <common/luax.hpp>

#include <objects/joint/types/gearjoint/gearjoint.hpp>
#include <objects/joint/wrap_joint.hpp>

namespace Wrap_GearJoint
{
    love::GearJoint* CheckGearJoint(lua_State* L, int index);

    int SetRatio(lua_State* L);

    int GetRatio(lua_State* L);

    int GetJoints(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_GearJoint
