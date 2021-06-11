#pragma once

#include "common/luax.h"
#include "distancejoint/distancejoint.h"

namespace Wrap_DistanceJoint
{
    int SetLength(lua_State* L);

    int GetLength(lua_State* L);

    int SetFrequency(lua_State* L);

    int GetFrequency(lua_State* L);

    int SetDampingRatio(lua_State* L);

    int GetDampingRatio(lua_State* L);

    int SetStiffness(lua_State* L);

    int GetStiffness(lua_State* L);

    int SetDamping(lua_State* L);

    int GetDamping(lua_State* L);

    love::DistanceJoint* CheckDistanceJoint(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_DistanceJoint
