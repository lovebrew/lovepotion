#pragma once

#include "common/luax.h"
#include "motorjoint/motorjoint.h"

namespace Wrap_MotorJoint
{
    love::MotorJoint* CheckMotorJoint(lua_State* L, int index);

    int SetLinearOffset(lua_State* L);

    int GetLinearOffset(lua_State* L);

    int SetAngularOffset(lua_State* L);

    int GetAngularOffset(lua_State* L);

    int SetMaxForce(lua_State* L);

    int GetMaxForce(lua_State* L);

    int SetMaxTorque(lua_State* L);

    int GetMaxTorque(lua_State* L);

    int SetCorrectionFactor(lua_State* L);

    int GetCorrectionFactor(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_MotorJoint
