#include "revolutejoint/wrap_revolutejoint.h"
#include "joint/wrap_joint.h"

using namespace love;

RevoluteJoint* Wrap_RevoluteJoint::CheckRevoluteJoint(lua_State* L, int index)
{
    RevoluteJoint* joint = Luax::CheckType<RevoluteJoint>(L, index);

    if (!joint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint!");

    return joint;
}

int Wrap_RevoluteJoint::GetJointAngle(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    lua_pushnumber(L, self->GetJointAngle());

    return 1;
}

int Wrap_RevoluteJoint::GetJointSpeed(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    lua_pushnumber(L, self->GetJointSpeed());

    return 1;
}

int Wrap_RevoluteJoint::SetMotorEnabled(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);
    bool enabled        = Luax::CheckBoolean(L, 2);

    self->SetMotorEnabled(enabled);

    return 0;
}

int Wrap_RevoluteJoint::IsMotorEnabled(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    Luax::PushBoolean(L, self->IsMotorEnabled());

    return 1;
}

int Wrap_RevoluteJoint::SetMaxMotorTorque(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);
    float torque        = luaL_checknumber(L, 2);

    self->SetMaxMotorTorque(torque);

    return 0;
}

int Wrap_RevoluteJoint::SetMotorSpeed(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);
    float speed         = luaL_checknumber(L, 2);

    self->SetMotorSpeed(speed);

    return 0;
}

int Wrap_RevoluteJoint::GetMotorSpeed(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    lua_pushnumber(L, self->GetMotorSpeed());

    return 1;
}

int Wrap_RevoluteJoint::GetMotorTorque(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);
    float invDt         = luaL_checknumber(L, 2);

    lua_pushnumber(L, self->GetMotorTorque(invDt));

    return 1;
}

int Wrap_RevoluteJoint::GetMaxMotorTorque(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    lua_pushnumber(L, self->GetMaxMotorTorque());

    return 1;
}

int Wrap_RevoluteJoint::SetLimitsEnabled(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);
    bool enabled        = Luax::CheckBoolean(L, 2);

    self->SetLimitsEnabled(enabled);

    return 0;
}

int Wrap_RevoluteJoint::AreLimitsEnabled(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    Luax::PushBoolean(L, self->AreLimitsEnabled());

    return 1;
}

int Wrap_RevoluteJoint::SetUpperLimit(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);
    float limit         = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { self->SetUpperLimit(limit); });

    return 0;
}

int Wrap_RevoluteJoint::SetLowerLimit(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);
    float limit         = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { self->SetLowerLimit(limit); });

    return 0;
}

int Wrap_RevoluteJoint::SetLimits(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    float lower = luaL_checknumber(L, 2);
    float upper = luaL_checknumber(L, 3);

    Luax::CatchException(L, [&]() { self->SetLimits(lower, upper); });

    return 0;
}

int Wrap_RevoluteJoint::GetLowerLimit(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    lua_pushnumber(L, self->GetLowerLimit());

    return 1;
}

int Wrap_RevoluteJoint::GetUpperLimit(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    lua_pushnumber(L, self->GetUpperLimit());

    return 1;
}

int Wrap_RevoluteJoint::GetLimits(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);
    lua_remove(L, 1);

    return self->GetLimits(L);
}

int Wrap_RevoluteJoint::GetReferenceAngle(lua_State* L)
{
    RevoluteJoint* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    lua_pushnumber(L, self->GetReferenceAngle());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "areLimitsEnabled",  Wrap_RevoluteJoint::AreLimitsEnabled  },
    { "getJointAngle",     Wrap_RevoluteJoint::GetJointAngle     },
    { "getJointSpeed",     Wrap_RevoluteJoint::GetJointSpeed     },
    { "getLimits",         Wrap_RevoluteJoint::GetLimits         },
    { "getLowerLimit",     Wrap_RevoluteJoint::GetLowerLimit     },
    { "getMaxMotorTorque", Wrap_RevoluteJoint::GetMaxMotorTorque },
    { "getMotorSpeed",     Wrap_RevoluteJoint::GetMotorSpeed     },
    { "getMotorTorque",    Wrap_RevoluteJoint::GetMotorTorque    },
    { "getReferenceAngle", Wrap_RevoluteJoint::GetReferenceAngle },
    { "getUpperLimit",     Wrap_RevoluteJoint::GetUpperLimit     },
    { "isMotorEnabled",    Wrap_RevoluteJoint::IsMotorEnabled    },
    { "setLimits",         Wrap_RevoluteJoint::SetLimits         },
    { "setLimitsEnabled",  Wrap_RevoluteJoint::SetLimitsEnabled  },
    { "setLowerLimit",     Wrap_RevoluteJoint::SetLowerLimit     },
    { "setMaxMotorTorque", Wrap_RevoluteJoint::SetMaxMotorTorque },
    { "setMotorEnabled",   Wrap_RevoluteJoint::SetMotorEnabled   },
    { "setMotorSpeed",     Wrap_RevoluteJoint::SetMotorSpeed     },
    { "setUpperLimit",     Wrap_RevoluteJoint::SetUpperLimit     },
    { 0,                   0                                     }
};
// clang-format on

int Wrap_RevoluteJoint::Register(lua_State* L)
{
    return Luax::RegisterType(L, &RevoluteJoint::type, Wrap_Joint::functions, functions, nullptr);
}
