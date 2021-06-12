#pragma once

#include "common/luax.h"
#include "revolutejoint/revolutejoint.h"

namespace Wrap_RevoluteJoint
{
    int GetJointAngle(lua_State* L);

    int GetJointSpeed(lua_State* L);

    int SetMotorEnabled(lua_State* L);

    int IsMotorEnabled(lua_State* L);

    int SetMaxMotorTorque(lua_State* L);

    int SetMotorSpeed(lua_State* L);

    int GetMotorSpeed(lua_State* L);

    int GetMotorTorque(lua_State* L);

    int GetMaxMotorTorque(lua_State* L);

    int SetLimitsEnabled(lua_State* L);

    int AreLimitsEnabled(lua_State* L);

    int SetUpperLimit(lua_State* L);

    int SetLowerLimit(lua_State* L);

    int SetLimits(lua_State* L);

    int GetLowerLimit(lua_State* L);

    int GetUpperLimit(lua_State* L);

    int GetLimits(lua_State* L);

    int GetReferenceAngle(lua_State* L);

    love::RevoluteJoint* CheckRevoluteJoint(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_RevoluteJoint
