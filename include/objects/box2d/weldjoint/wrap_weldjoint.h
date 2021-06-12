#pragma once

#include "common/luax.h"
#include "weldjoint/weldjoint.h"

namespace Wrap_WeldJoint
{
    int SetFrequency(lua_State* L);

    int GetFrequency(lua_State* L);

    int SetDampingRatio(lua_State* L);

    int GetDampingRatio(lua_State* L);

    int SetStiffness(lua_State* L);

    int GetStiffness(lua_State* L);

    int SetDamping(lua_State* L);

    int GetDamping(lua_State* L);

    int GetReferenceAngle(lua_State* L);

    love::WeldJoint* CheckWeldJoint(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_WeldJoint
