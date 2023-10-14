#include <objects/joint/types/motorjoint/wrap_motorjoint.hpp>

using namespace love;

MotorJoint* Wrap_MotorJoint::CheckMotorJoint(lua_State* L, int index)
{
    auto* motorJoint = luax::CheckType<MotorJoint>(L, index);

    if (!motorJoint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint.");

    return motorJoint;
}

int Wrap_MotorJoint::SetLinearOffset(lua_State* L)
{
    auto* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    self->SetLinearOffset(x, y);

    return 0;
}

int Wrap_MotorJoint::GetLinearOffset(lua_State* L)
{
    auto* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);

    return self->GetLinearOffset(L);
}

int Wrap_MotorJoint::SetAngularOffset(lua_State* L)
{
    auto* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);

    float angularOffset = luaL_checknumber(L, 2);
    self->SetAngularOffset(angularOffset);

    return 0;
}

int Wrap_MotorJoint::GetAngularOffset(lua_State* L)
{
    auto* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);

    lua_pushnumber(L, self->GetAngularOffset());

    return 1;
}

int Wrap_MotorJoint::SetMaxForce(lua_State* L)
{
    auto* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);

    float force = luaL_checknumber(L, 2);
    luax::CatchException(L, [&]() { self->SetMaxForce(force); });

    return 0;
}

int Wrap_MotorJoint::GetMaxForce(lua_State* L)
{
    auto* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);

    lua_pushnumber(L, self->GetMaxForce());

    return 1;
}

int Wrap_MotorJoint::SetMaxTorque(lua_State* L)
{
    auto* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);

    float torque = luaL_checknumber(L, 2);
    luax::CatchException(L, [&]() { self->SetMaxTorque(torque); });

    return 0;
}

int Wrap_MotorJoint::GetMaxTorque(lua_State* L)
{
    auto* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);

    lua_pushnumber(L, self->GetMaxTorque());

    return 1;
}

int Wrap_MotorJoint::SetCorrectionFactor(lua_State* L)
{
    auto* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);

    float factor = luaL_checknumber(L, 2);
    luax::CatchException(L, [&]() { self->SetCorrectionFactor(factor); });

    return 0;
}

int Wrap_MotorJoint::GetCorrectionFactor(lua_State* L)
{
    auto* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);

    lua_pushnumber(L, self->GetCorrectionFactor());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "setLinearOffset",     Wrap_MotorJoint::SetLinearOffset     },
    { "getLinearOffset",     Wrap_MotorJoint::GetLinearOffset     },
    { "setAngularOffset",    Wrap_MotorJoint::SetAngularOffset    },
    { "getAngularOffset",    Wrap_MotorJoint::GetAngularOffset    },
    { "setMaxForce",         Wrap_MotorJoint::SetMaxForce         },
    { "getMaxForce",         Wrap_MotorJoint::GetMaxForce         },
    { "setMaxTorque",        Wrap_MotorJoint::SetMaxTorque        },
    { "getMaxTorque",        Wrap_MotorJoint::GetMaxTorque        },
    { "setCorrectionFactor", Wrap_MotorJoint::SetCorrectionFactor },
    { "getCorrectionFactor", Wrap_MotorJoint::GetCorrectionFactor }
};
// clang-format on

int Wrap_MotorJoint::Register(lua_State* L)
{
    return luax::RegisterType(L, &MotorJoint::type, Wrap_Joint::jointFunctions, functions);
}
