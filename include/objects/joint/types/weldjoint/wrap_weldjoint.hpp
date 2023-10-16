#pragma once

#include <common/luax.hpp>

#include <objects/joint/types/weldjoint/weldjoint.hpp>
#include <objects/joint/wrap_joint.hpp>

namespace Wrap_WeldJoint
{
    love::WeldJoint* CheckWeldJoint(lua_State* L, int index);

    int SetStiffness(lua_State* L);

    int GetStiffness(lua_State* L);

    int SetDamping(lua_State* L);

    int GetDamping(lua_State* L);

    int GetReferenceAngle(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_WeldJoint
