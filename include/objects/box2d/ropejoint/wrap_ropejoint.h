#pragma once

#include "common/luax.h"
#include "ropejoint/ropejoint.h"

namespace Wrap_RopeJoint
{
    int GetMaxLength(lua_State* L);

    int SetMaxLength(lua_State* L);

    love::RopeJoint* CheckRopeJoint(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_RopeJoint
