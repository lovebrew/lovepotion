#pragma once

#include "common/luax.h"
#include "gearjoint/gearjoint.h"

namespace Wrap_GearJoint
{
    int SetRatio(lua_State* L);

    int GetRatio(lua_State* L);

    int GetJoints(lua_State* L);

    love::GearJoint* CheckGearJoint(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_GearJoint
