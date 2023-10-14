#include <objects/joint/types/revolutejoint/wrap_revolutejoint.hpp>

using namespace love;

RevoluteJoint* Wrap_RevoluteJoint::CheckRevoluteJoint(lua_State* L, int index)
{
    auto* revoluteJoint = luax::CheckType<RevoluteJoint>(L, index);

    if (!revoluteJoint->IsValid())
        luaL_error(L, "Attempt to use destroyed RevoluteJoint.");

    return revoluteJoint;
}

int Wrap_RevoluteJoint::GetJointAngle(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    lua_pushnumber(L, self->GetJointAngle());

    return 1;
}

int Wrap_RevoluteJoint::GetJointSpeed(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    lua_pushnumber(L, self->GetJointSpeed());

    return 1;
}

int Wrap_RevoluteJoint::SetMotorEnabled(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    bool enable = luax::CheckBoolean(L, 2);
    self->SetMotorEnabled(enable);

    return 0;
}

int Wrap_RevoluteJoint::IsMotorEnabled(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    luax::PushBoolean(L, self->IsMotorEnabled());

    return 1;
}

int Wrap_RevoluteJoint::SetMaxMotorTorque(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    float maxMotorTorque = luaL_checknumber(L, 2);

    self->SetMaxMotorTorque(maxMotorTorque);

    return 0;
}

int Wrap_RevoluteJoint::SetMotorSpeed(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    float motorSpeed = luaL_checknumber(L, 2);

    self->SetMotorSpeed(motorSpeed);

    return 0;
}

int Wrap_RevoluteJoint::GetMotorSpeed(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    lua_pushnumber(L, self->GetMotorSpeed());

    return 1;
}

int Wrap_RevoluteJoint::GetMotorTorque(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    float dt = luaL_checknumber(L, 2);

    lua_pushnumber(L, self->GetMotorTorque(dt));

    return 1;
}

int Wrap_RevoluteJoint::GetMaxMotorTorque(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    lua_pushnumber(L, self->GetMaxMotorTorque());

    return 1;
}

int Wrap_RevoluteJoint::SetLimitsEnabled(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    bool enable = luax::CheckBoolean(L, 2);

    self->SetLimitsEnabled(enable);

    return 0;
}

int Wrap_RevoluteJoint::AreLimitsEnabled(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    luax::PushBoolean(L, self->AreLimitsEnabled());

    return 1;
}

int Wrap_RevoluteJoint::SetUpperLimit(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    float upperLimit = luaL_checknumber(L, 2);

    luax::CatchException(L, [&]() { self->SetUpperLimit(upperLimit); });

    return 0;
}

int Wrap_RevoluteJoint::SetLowerLimit(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    float lowerLimit = luaL_checknumber(L, 2);

    luax::CatchException(L, [&]() { self->SetLowerLimit(lowerLimit); });

    return 0;
}

int Wrap_RevoluteJoint::SetLimits(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    float lowerLimit = luaL_checknumber(L, 2);
    float upperLimit = luaL_checknumber(L, 3);

    luax::CatchException(L, [&]() { self->SetLimits(lowerLimit, upperLimit); });

    return 0;
}

int Wrap_RevoluteJoint::GetUpperLimit(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    lua_pushnumber(L, self->GetUpperLimit());

    return 1;
}

int Wrap_RevoluteJoint::GetLowerLimit(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    lua_pushnumber(L, self->GetLowerLimit());

    return 1;
}

int Wrap_RevoluteJoint::GetLimits(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);
    lua_remove(L, 1);

    return self->GetLimits(L);
}

int Wrap_RevoluteJoint::GetReferenceAngle(lua_State* L)
{
    auto* self = Wrap_RevoluteJoint::CheckRevoluteJoint(L, 1);

    lua_pushnumber(L, self->GetReferenceAngle());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getJointAngle",     Wrap_RevoluteJoint::GetJointAngle     },
    { "getJointSpeed",     Wrap_RevoluteJoint::GetJointSpeed     },
    { "setMotorEnabled",   Wrap_RevoluteJoint::SetMotorEnabled   },
    { "isMotorEnabled",    Wrap_RevoluteJoint::IsMotorEnabled    },
    { "setMaxMotorTorque", Wrap_RevoluteJoint::SetMaxMotorTorque },
    { "setMotorSpeed",     Wrap_RevoluteJoint::SetMotorSpeed     },
    { "getMotorSpeed",     Wrap_RevoluteJoint::GetMotorSpeed     },
    { "getMotorTorque",    Wrap_RevoluteJoint::GetMotorTorque    },
    { "getMaxMotorTorque", Wrap_RevoluteJoint::GetMaxMotorTorque },
    { "setLimitsEnabled",  Wrap_RevoluteJoint::SetLimitsEnabled  },
    { "areLimitsEnabled",  Wrap_RevoluteJoint::AreLimitsEnabled  },
    { "setUpperLimit",     Wrap_RevoluteJoint::SetUpperLimit     },
    { "setLowerLimit",     Wrap_RevoluteJoint::SetLowerLimit     },
    { "setLimits",         Wrap_RevoluteJoint::SetLimits         },
    { "getUpperLimit",     Wrap_RevoluteJoint::GetUpperLimit     },
    { "getLowerLimit",     Wrap_RevoluteJoint::GetLowerLimit     },
    { "getLimits",         Wrap_RevoluteJoint::GetLimits         },
    { "getReferenceAngle", Wrap_RevoluteJoint::GetReferenceAngle }
};
// clang-format on

int Wrap_RevoluteJoint::Register(lua_State* L)
{
    return luax::RegisterType(L, &RevoluteJoint::type, Wrap_Joint::jointFunctions, functions);
}
