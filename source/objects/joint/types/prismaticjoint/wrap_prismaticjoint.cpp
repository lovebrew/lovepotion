#include <objects/joint/types/prismaticjoint/wrap_prismaticjoint.hpp>

using namespace love;

PrismaticJoint* Wrap_PrismaticJoint::CheckPrismaticJoint(lua_State* L, int index)
{
    auto* prismaticJoint = luax::CheckType<PrismaticJoint>(L, index);

    if (!prismaticJoint->IsValid())
        luaL_error(L, "Attempt to use destroyed PrismaticJoint.");

    return prismaticJoint;
}

int Wrap_PrismaticJoint::GetJointTranslation(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    lua_pushnumber(L, self->GetJointTranslation());

    return 1;
}

int Wrap_PrismaticJoint::GetJointSpeed(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    lua_pushnumber(L, self->GetJointSpeed());

    return 1;
}

int Wrap_PrismaticJoint::SetMotorEnabled(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    bool enable = luax::CheckBoolean(L, 2);
    self->SetMotorEnabled(enable);

    return 0;
}

int Wrap_PrismaticJoint::IsMotorEnabled(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    luax::PushBoolean(L, self->IsMotorEnabled());

    return 1;
}

int Wrap_PrismaticJoint::SetMaxMotorForce(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    float force = luaL_checknumber(L, 2);
    self->SetMaxMotorForce(force);

    return 0;
}

int Wrap_PrismaticJoint::SetMotorSpeed(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    float speed = luaL_checknumber(L, 2);
    self->SetMotorSpeed(speed);

    return 0;
}

int Wrap_PrismaticJoint::GetMotorSpeed(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    lua_pushnumber(L, self->GetMotorSpeed());

    return 1;
}

int Wrap_PrismaticJoint::GetMotorForce(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    float inverseDelta = luaL_checknumber(L, 2);
    lua_pushnumber(L, self->GetMotorForce(inverseDelta));

    return 1;
}

int Wrap_PrismaticJoint::GetMaxMotorForce(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    lua_pushnumber(L, self->GetMaxMotorForce());

    return 1;
}

int Wrap_PrismaticJoint::SetLimitsEnabled(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    bool enable = luax::CheckBoolean(L, 2);
    self->SetLimitsEnabled(enable);

    return 0;
}

int Wrap_PrismaticJoint::AreLimitsEnabled(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    luax::PushBoolean(L, self->AreLimitsEnabled());

    return 1;
}

int Wrap_PrismaticJoint::SetUpperLimit(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    float limit = luaL_checknumber(L, 2);
    self->SetUpperLimit(limit);

    return 0;
}

int Wrap_PrismaticJoint::SetLowerLimit(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    float limit = luaL_checknumber(L, 2);
    self->SetLowerLimit(limit);

    return 0;
}

int Wrap_PrismaticJoint::SetLimits(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    float lower = luaL_checknumber(L, 2);
    float upper = luaL_checknumber(L, 3);

    self->SetLimits(lower, upper);

    return 0;
}

int Wrap_PrismaticJoint::GetLowerLimit(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    lua_pushnumber(L, self->GetLowerLimit());

    return 1;
}

int Wrap_PrismaticJoint::GetUpperLimit(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    lua_pushnumber(L, self->GetUpperLimit());

    return 1;
}

int Wrap_PrismaticJoint::GetLimits(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);
    lua_remove(L, 1);

    return self->GetLimits(L);
}

int Wrap_PrismaticJoint::GetAxis(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);
    lua_remove(L, 1);

    return self->GetAxis(L);
}

int Wrap_PrismaticJoint::GetReferenceAngle(lua_State* L)
{
    auto* self = Wrap_PrismaticJoint::CheckPrismaticJoint(L, 1);

    lua_pushnumber(L, self->GetReferenceAngle());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getJointTranslation", Wrap_PrismaticJoint::GetJointTranslation },
    { "getJointSpeed",       Wrap_PrismaticJoint::GetJointSpeed       },
    { "setMotorEnabled",     Wrap_PrismaticJoint::SetMotorEnabled     },
    { "isMotorEnabled",      Wrap_PrismaticJoint::IsMotorEnabled      },
    { "setMaxMotorForce",    Wrap_PrismaticJoint::SetMaxMotorForce    },
    { "setMotorSpeed",       Wrap_PrismaticJoint::SetMotorSpeed       },
    { "getMotorSpeed",       Wrap_PrismaticJoint::GetMotorSpeed       },
    { "getMotorForce",       Wrap_PrismaticJoint::GetMotorForce       },
    { "getMaxMotorForce",    Wrap_PrismaticJoint::GetMaxMotorForce    },
    { "setLimitsEnabled",    Wrap_PrismaticJoint::SetLimitsEnabled    },
    { "areLimitsEnabled",    Wrap_PrismaticJoint::AreLimitsEnabled    },
    { "setUpperLimit",       Wrap_PrismaticJoint::SetUpperLimit       },
    { "setLowerLimit",       Wrap_PrismaticJoint::SetLowerLimit       },
    { "setLimits",           Wrap_PrismaticJoint::SetLimits           },
    { "getUpperLimit",       Wrap_PrismaticJoint::GetUpperLimit       },
    { "getLowerLimit",       Wrap_PrismaticJoint::GetLowerLimit       },
    { "getLimits",           Wrap_PrismaticJoint::GetLimits           },
    { "getAxis",             Wrap_PrismaticJoint::GetAxis             },
    { "getReferenceAngle",   Wrap_PrismaticJoint::GetReferenceAngle   }
};
// clang-format on

int Wrap_PrismaticJoint::Register(lua_State* L)
{
    return luax::RegisterType(L, &PrismaticJoint::type, Wrap_Joint::jointFunctions, functions);
}
