#pragma once

#include <common/luax.hpp>

#include <objects/joint/types/distancejoint/distancejoint.hpp>
#include <objects/joint/wrap_joint.hpp>

namespace Wrap_DistanceJoint
{
    love::DistanceJoint* CheckDistanceJoint(lua_State* L, int index);

    int SetLength(lua_State* L);

    int GetLength(lua_State* L);

    int SetStiffness(lua_State* L);

    int GetStiffness(lua_State* L);

    int SetDamping(lua_State* L);

    int GetDamping(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_DistanceJoint
