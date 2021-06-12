#pragma once

#include "common/luax.h"
#include "pulleyjoint/pulleyjoint.h"

namespace Wrap_PulleyJoint
{
    int GetGroundAnchors(lua_State* L);

    int GetLengthA(lua_State* L);

    int GetLengthB(lua_State* L);

    int GetRatio(lua_State* L);

    love::PulleyJoint* CheckPulleyJoint(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_PulleyJoint
