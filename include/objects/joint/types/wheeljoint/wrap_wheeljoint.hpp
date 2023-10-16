#pragma once

#include <common/luax.hpp>

#include <objects/joint/types/wheeljoint/wheeljoint.hpp>
#include <objects/joint/wrap_joint.hpp>

namespace Wrap_WheelJoint
{
    love::WheelJoint* CheckWheelJoint(lua_State* L, int index);

    int GetJointTranslation(lua_State* L);

    int GetJointSpeed(lua_State* L);

    int SetMotorEnabled(lua_State* L);

    int IsMotorEnabled(lua_State* L);

    int SetMotorSpeed(lua_State* L);

    int GetMotorSpeed(lua_State* L);

    int SetMaxMotorTorque(lua_State* L);

    int GetMaxMotorTorque(lua_State* L);

    int GetMotorTorque(lua_State* L);

    int SetStiffness(lua_State* L);

    int GetStiffness(lua_State* L);

    int SetDamping(lua_State* L);

    int GetDamping(lua_State* L);

    int GetAxis(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_WheelJoint
