#include "motorjoint/wrap_motorjoint.h"
#include "joint/wrap_joint.h"

using namespace love;

MotorJoint* Wrap_MotorJoint::CheckMotorJoint(lua_State* L, int index)
{
    MotorJoint* joint = Luax::CheckType<MotorJoint>(L, index);

    if (!joint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint!");

    return joint;
}

int Wrap_MotorJoint::SetLinearOffset(lua_State* L)
{
    MotorJoint* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    self->SetLinearOffset(x, y);

    return 0;
}

int Wrap_MotorJoint::GetLinearOffset(lua_State* L)
{
    MotorJoint* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);
    // lua_remove(L, 1); not needed?

    return self->GetLinearOffset(L);
}

int Wrap_MotorJoint::SetAngularOffset(lua_State* L)
{
    MotorJoint* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);
    float offset     = luaL_checknumber(L, 2);

    self->SetAngularOffset(offset);

    return 0;
}

int Wrap_MotorJoint::GetAngularOffset(lua_State* L)
{
    MotorJoint* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);

    lua_pushnumber(L, self->GetAngularOffset());

    return 1;
}

int Wrap_MotorJoint::SetMaxForce(lua_State* L)
{
    MotorJoint* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);
    float maxForce   = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { self->SetMaxForce(maxForce); });

    return 0;
}

int Wrap_MotorJoint::GetMaxForce(lua_State* L)
{
    MotorJoint* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);

    lua_pushnumber(L, self->GetMaxForce());

    return 1;
}

int Wrap_MotorJoint::SetMaxTorque(lua_State* L)
{
    MotorJoint* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);
    float maxTorque  = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { self->SetMaxTorque(maxTorque); });

    return 0;
}

int Wrap_MotorJoint::GetMaxTorque(lua_State* L)
{
    MotorJoint* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);

    lua_pushnumber(L, self->GetMaxTorque());

    return 1;
}

int Wrap_MotorJoint::SetCorrectionFactor(lua_State* L)
{
    MotorJoint* self       = Wrap_MotorJoint::CheckMotorJoint(L, 1);
    float correctionFactor = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { self->SetCorrectionFactor(correctionFactor); });

    return 0;
}

int Wrap_MotorJoint::GetCorrectionFactor(lua_State* L)
{
    MotorJoint* self = Wrap_MotorJoint::CheckMotorJoint(L, 1);

    lua_pushnumber(L, self->GetCorrectionFactor());

    return 1;
}

int Wrap_MotorJoint::Register(lua_State* L)
{
    luaL_Reg funcs[] = { { "setLinearOffset", SetLinearOffset },
                         { "getLinearOffset", GetLinearOffset },
                         { "setAngularOffset", SetAngularOffset },
                         { "getAngularOffset", GetAngularOffset },
                         { "setMaxForce", SetMaxForce },
                         { "getMaxForce", GetMaxForce },
                         { "setMaxTorque", SetMaxTorque },
                         { "getMaxTorque", GetMaxTorque },
                         { "setCorrectionFactor", SetCorrectionFactor },
                         { "getCorrectionFactor", GetCorrectionFactor },
                         { 0, 0 } };

    return Luax::RegisterType(L, &MotorJoint::type, Wrap_Joint::functions, funcs, nullptr);
}
