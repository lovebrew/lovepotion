#pragma once

#include <common/luax.hpp>

#include <objects/joint/types/ropejoint/ropejoint.hpp>
#include <objects/joint/wrap_joint.hpp>

namespace Wrap_RopeJoint
{
    love::RopeJoint* CheckRopeJoint(lua_State* L, int index);

    int GetMaxLength(lua_State* L);

    int SetMaxLength(lua_State* L);

    int Register(lua_State* L);
}
