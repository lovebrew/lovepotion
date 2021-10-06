#include "frictionjoint/wrap_frictionjoint.h"
#include "joint/wrap_joint.h"

using namespace love;

FrictionJoint* Wrap_FrictionJoint::CheckFrictionJoint(lua_State* L, int index)
{
    FrictionJoint* joint = Luax::CheckType<FrictionJoint>(L, index);

    if (!joint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint!");

    return joint;
}

int Wrap_FrictionJoint::SetMaxForce(lua_State* L)
{
    FrictionJoint* self = Wrap_FrictionJoint::CheckFrictionJoint(L, 1);
    float maxForce      = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { self->SetMaxForce(maxForce); });

    return 0;
}

int Wrap_FrictionJoint::GetMaxForce(lua_State* L)
{
    FrictionJoint* self = Wrap_FrictionJoint::CheckFrictionJoint(L, 1);

    lua_pushnumber(L, self->GetMaxForce());

    return 1;
}

int Wrap_FrictionJoint::SetMaxTorque(lua_State* L)
{
    FrictionJoint* self = Wrap_FrictionJoint::CheckFrictionJoint(L, 1);
    float maxTorque     = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { self->SetMaxTorque(maxTorque); });

    return 0;
}

int Wrap_FrictionJoint::GetMaxTorque(lua_State* L)
{
    FrictionJoint* self = Wrap_FrictionJoint::CheckFrictionJoint(L, 1);

    lua_pushnumber(L, self->GetMaxTorque());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getMaxForce",  Wrap_FrictionJoint::GetMaxForce  },
    { "getMaxTorque", Wrap_FrictionJoint::GetMaxTorque },
    { "setMaxForce",  Wrap_FrictionJoint::SetMaxForce  },
    { "setMaxTorque", Wrap_FrictionJoint::SetMaxTorque },
    { 0,              0                                }
};
// clang-format on

int Wrap_FrictionJoint::Register(lua_State* L)
{
    return Luax::RegisterType(L, &FrictionJoint::type, Wrap_Joint::functions, functions, nullptr);
}
