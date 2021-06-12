#pragma once

#include "common/luax.h"
#include "wheeljoint/wheeljoint.h"

namespace Wrap_WheelJoint
{
    int GetJointTranslation(lua_State* L);

    int GetJointSpeed(lua_State* L);

    int SetMotorEnabled(lua_State* L);

    int IsMotorEnabled(lua_State* L);

    int SetMotorSpeed(lua_State* L);

    int GetMotorSpeed(lua_State* L);

    int SetMaxMotorTorque(lua_State* L);

    int GetMaxMotorTorque(lua_State* L);

    int GetMotorTorque(lua_State* L);

    int SetFrequency(lua_State* L);

    int GetFrequency(lua_State* L);

    int SetDampingRatio(lua_State* L);

    int GetDampingRatio(lua_State* L);

    int SetStiffness(lua_State* L);

    int GetStiffness(lua_State* L);

    int SetDamping(lua_State* L);

    int GetDamping(lua_State* L);

    int GetAxis(lua_State* L);

    love::WheelJoint* CheckWheelJoint(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_WheelJoint
