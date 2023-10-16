#include <objects/joint/types/frictionjoint/wrap_frictionjoint.hpp>

using namespace love;

FrictionJoint* Wrap_FrictionJoint::CheckFrictionJoint(lua_State* L, int index)
{
    auto* frictionJoint = luax::CheckType<FrictionJoint>(L, index);

    if (!frictionJoint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint.");

    return frictionJoint;
}

int Wrap_FrictionJoint::SetMaxForce(lua_State* L)
{
    auto* self = Wrap_FrictionJoint::CheckFrictionJoint(L, 1);

    float maxForce = luaL_checknumber(L, 2);
    self->SetMaxForce(maxForce);

    return 0;
}

int Wrap_FrictionJoint::GetMaxForce(lua_State* L)
{
    auto* self = Wrap_FrictionJoint::CheckFrictionJoint(L, 1);

    lua_pushnumber(L, self->GetMaxForce());

    return 1;
}

int Wrap_FrictionJoint::SetMaxTorque(lua_State* L)
{
    auto* self = Wrap_FrictionJoint::CheckFrictionJoint(L, 1);

    float maxTorque = luaL_checknumber(L, 2);
    self->SetMaxTorque(maxTorque);

    return 0;
}

int Wrap_FrictionJoint::GetMaxTorque(lua_State* L)
{
    auto* self = Wrap_FrictionJoint::CheckFrictionJoint(L, 1);

    lua_pushnumber(L, self->GetMaxTorque());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "setMaxForce",  Wrap_FrictionJoint::SetMaxForce  },
    { "getMaxForce",  Wrap_FrictionJoint::GetMaxForce  },
    { "setMaxTorque", Wrap_FrictionJoint::SetMaxTorque },
    { "getMaxTorque", Wrap_FrictionJoint::GetMaxTorque }
};
// clang-format on

int Wrap_FrictionJoint::Register(lua_State* L)
{
    return luax::RegisterType(L, &FrictionJoint::type, Wrap_Joint::jointFunctions, functions);
}
