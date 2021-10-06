#include "prismaticjoint/wrap_prismaticjoint.h"
#include "joint/wrap_joint.h"

using namespace love;

PrismaticJoint* Wrap_PrismaticJoint::CheckPrismaticJoint(lua_State* L, int index)
{
    PrismaticJoint* joint = Luax::CheckType<PrismaticJoint>(L, index);

    if (!joint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint!");

    return joint;
}

int Wrap_PrismaticJoint::GetJointTranslation(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    lua_pushnumber(L, self->GetJointTranslation());

    return 1;
}

int Wrap_PrismaticJoint::GetJointSpeed(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    lua_pushnumber(L, self->GetJointSpeed());

    return 1;
}

int Wrap_PrismaticJoint::SetMotorEnabled(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);
    bool enabled         = Luax::CheckBoolean(L, 2);

    self->SetMotorEnabled(enabled);

    return 0;
}

int Wrap_PrismaticJoint::IsMotorEnabled(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    Luax::PushBoolean(L, self->IsMotorEnabled());

    return 1;
}

int Wrap_PrismaticJoint::SetMaxMotorForce(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);
    float maxForce       = luaL_checknumber(L, 2);

    self->SetMaxMotorForce(maxForce);

    return 0;
}

int Wrap_PrismaticJoint::SetMotorSpeed(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);
    float motorSpeed     = luaL_checknumber(L, 2);

    self->SetMotorSpeed(motorSpeed);

    return 0;
}

int Wrap_PrismaticJoint::GetMotorSpeed(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    lua_pushnumber(L, self->GetMotorSpeed());

    return 1;
}

int Wrap_PrismaticJoint::GetMotorForce(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);
    float invDt          = luaL_checknumber(L, 2);

    lua_pushnumber(L, self->GetMotorForce(invDt));

    return 1;
}

int Wrap_PrismaticJoint::GetMaxMotorForce(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    lua_pushnumber(L, self->GetMaxMotorForce());

    return 1;
}

int Wrap_PrismaticJoint::SetLimitsEnabled(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);
    bool enable          = Luax::CheckBoolean(L, 2);

    self->SetLimitsEnabled(enable);

    return 0;
}

int Wrap_PrismaticJoint::AreLimitsEnabled(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    Luax::PushBoolean(L, self->AreLimitsEnabled());

    return 1;
}

int Wrap_PrismaticJoint::SetUpperLimit(lua_State* L)
{
    PrismaticJoint* t = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);
    float limit       = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { t->SetUpperLimit(limit); });

    return 0;
}

int Wrap_PrismaticJoint::SetLowerLimit(lua_State* L)
{
    PrismaticJoint* t = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);
    float limit       = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { t->SetLowerLimit(limit); });

    return 0;
}

int Wrap_PrismaticJoint::SetLimits(lua_State* L)
{
    PrismaticJoint* t = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    float lower = luaL_checknumber(L, 2);
    float upper = luaL_checknumber(L, 3);

    Luax::CatchException(L, [&]() { t->SetLimits(lower, upper); });

    return 0;
}

int Wrap_PrismaticJoint::GetLowerLimit(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    lua_pushnumber(L, self->GetLowerLimit());

    return 1;
}

int Wrap_PrismaticJoint::GetUpperLimit(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    lua_pushnumber(L, self->GetUpperLimit());

    return 1;
}

int Wrap_PrismaticJoint::GetLimits(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);
    lua_remove(L, 1);

    return self->GetLimits(L);
}

int Wrap_PrismaticJoint::GetAxis(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);
    lua_remove(L, 1);

    return self->GetAxis(L);
}

int Wrap_PrismaticJoint::GetReferenceAngle(lua_State* L)
{
    PrismaticJoint* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    lua_pushnumber(L, self->GetReferenceAngle());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "areLimitsEnabled",    Wrap_PrismaticJoint::AreLimitsEnabled    },
    { "getAxis",             Wrap_PrismaticJoint::GetAxis             },
    { "getJointSpeed",       Wrap_PrismaticJoint::GetJointSpeed       },
    { "getJointTranslation", Wrap_PrismaticJoint::GetJointTranslation },
    { "getLimits",           Wrap_PrismaticJoint::GetLimits           },
    { "getLowerLimit",       Wrap_PrismaticJoint::GetLowerLimit       },
    { "getMaxMotorForce",    Wrap_PrismaticJoint::GetMaxMotorForce    },
    { "getMotorForce",       Wrap_PrismaticJoint::GetMotorForce       },
    { "getMotorSpeed",       Wrap_PrismaticJoint::GetMotorSpeed       },
    { "getReferenceAngle",   Wrap_PrismaticJoint::GetReferenceAngle   },
    { "getUpperLimit",       Wrap_PrismaticJoint::GetUpperLimit       },
    { "isMotorEnabled",      Wrap_PrismaticJoint::IsMotorEnabled      },
    { "setLimits",           Wrap_PrismaticJoint::SetLimits           },
    { "setLimitsEnabled",    Wrap_PrismaticJoint::SetLimitsEnabled    },
    { "setLowerLimit",       Wrap_PrismaticJoint::SetLowerLimit       },
    { "setMaxMotorForce",    Wrap_PrismaticJoint::SetMaxMotorForce    },
    { "setMotorEnabled",     Wrap_PrismaticJoint::SetMotorEnabled     },
    { "setMotorSpeed",       Wrap_PrismaticJoint::SetMotorSpeed       },
    { "setUpperLimit",       Wrap_PrismaticJoint::SetUpperLimit       },
    { 0,                     0                                        }
};
// clang-format on

int Wrap_PrismaticJoint::Register(lua_State* L)
{
    return Luax::RegisterType(L, &PrismaticJoint::type, Wrap_Joint::functions, functions, nullptr);
}
