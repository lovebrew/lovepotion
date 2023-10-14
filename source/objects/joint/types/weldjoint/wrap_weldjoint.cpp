#include <objects/joint/types/weldjoint/wrap_weldjoint.hpp>

using namespace love;

WeldJoint* Wrap_WeldJoint::CheckWeldJoint(lua_State* L, int index)
{
    auto* weldJoint = luax::CheckType<WeldJoint>(L, index);

    if (!weldJoint->IsValid())
        luaL_error(L, "Attempt to use destroyed weld joint.");

    return weldJoint;
}

int Wrap_WeldJoint::SetStiffness(lua_State* L)
{
    auto* self      = Wrap_WeldJoint::CheckWeldJoint(L, 1);
    float stiffness = (float)luaL_checknumber(L, 2);

    self->SetStiffness(stiffness);

    return 0;
}

int Wrap_WeldJoint::GetStiffness(lua_State* L)
{
    auto* self = Wrap_WeldJoint::CheckWeldJoint(L, 1);

    lua_pushnumber(L, self->GetStiffness());

    return 1;
}

int Wrap_WeldJoint::SetDamping(lua_State* L)
{
    auto* self  = Wrap_WeldJoint::CheckWeldJoint(L, 1);
    float ratio = (float)luaL_checknumber(L, 2);

    self->SetDamping(ratio);

    return 0;
}

int Wrap_WeldJoint::GetDamping(lua_State* L)
{
    auto* self = Wrap_WeldJoint::CheckWeldJoint(L, 1);

    lua_pushnumber(L, self->GetDamping());

    return 1;
}

int Wrap_WeldJoint::GetReferenceAngle(lua_State* L)
{
    auto* self = Wrap_WeldJoint::CheckWeldJoint(L, 1);

    lua_pushnumber(L, self->GetReferenceAngle());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "setStiffness",      Wrap_WeldJoint::SetStiffness      },
    { "getStiffness",      Wrap_WeldJoint::GetStiffness      },
    { "setDamping",        Wrap_WeldJoint::SetDamping        },
    { "getDamping",        Wrap_WeldJoint::GetDamping        },
    { "getReferenceAngle", Wrap_WeldJoint::GetReferenceAngle }
};
// clang-format on

int Wrap_WeldJoint::Register(lua_State* L)
{
    return luax::RegisterType(L, &WeldJoint::type, Wrap_Joint::jointFunctions, functions);
}
