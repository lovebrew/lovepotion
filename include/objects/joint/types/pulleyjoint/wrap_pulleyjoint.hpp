#pragma once

#include <common/luax.hpp>

#include <objects/joint/types/pulleyjoint/pulleyjoint.hpp>
#include <objects/joint/wrap_joint.hpp>

namespace Wrap_PulleyJoint
{
    love::PulleyJoint* CheckPulleyJoint(lua_State* L, int index);

    int GetGroundAnchors(lua_State* L);

    int GetLengthA(lua_State* L);

    int GetLengthB(lua_State* L);

    int GetRatio(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_PulleyJoint
