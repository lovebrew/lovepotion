#include "weldjoint/wrap_weldjoint.h"
#include "joint/wrap_joint.h"

using namespace love;

WeldJoint* Wrap_WeldJoint::CheckWeldJoint(lua_State* L, int index)
{
    WeldJoint* joint = Luax::CheckType<WeldJoint>(L, index);

    if (!joint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint!");

    return joint;
}

int Wrap_WeldJoint::SetFrequency(lua_State* L)
{
    WeldJoint* self = Wrap_WeldJoint::CheckWeldJoint(L, 1);
    float frequency = luaL_checknumber(L, 2);

    self->SetFrequency(frequency);

    return 0;
}

int Wrap_WeldJoint::GetFrequency(lua_State* L)
{
    WeldJoint* self = Wrap_WeldJoint::CheckWeldJoint(L, 1);

    lua_pushnumber(L, self->GetFrequency());

    return 1;
}

int Wrap_WeldJoint::SetDampingRatio(lua_State* L)
{
    WeldJoint* self = Wrap_WeldJoint::CheckWeldJoint(L, 1);
    float ratio     = luaL_checknumber(L, 2);

    self->SetDampingRatio(ratio);

    return 0;
}

int Wrap_WeldJoint::GetDampingRatio(lua_State* L)
{
    WeldJoint* self = Wrap_WeldJoint::CheckWeldJoint(L, 1);

    lua_pushnumber(L, self->GetDampingRatio());

    return 1;
}

int Wrap_WeldJoint::SetStiffness(lua_State* L)
{
    WeldJoint* self = Wrap_WeldJoint::CheckWeldJoint(L, 1);
    float stiffness = luaL_checknumber(L, 2);

    self->SetStiffness(stiffness);

    return 0;
}

int Wrap_WeldJoint::GetStiffness(lua_State* L)
{
    WeldJoint* self = Wrap_WeldJoint::CheckWeldJoint(L, 1);

    lua_pushnumber(L, self->GetStiffness());

    return 1;
}

int Wrap_WeldJoint::SetDamping(lua_State* L)
{
    WeldJoint* self = Wrap_WeldJoint::CheckWeldJoint(L, 1);
    float damping   = luaL_checknumber(L, 2);

    self->SetDamping(damping);

    return 0;
}

int Wrap_WeldJoint::GetDamping(lua_State* L)
{
    WeldJoint* self = Wrap_WeldJoint::CheckWeldJoint(L, 1);

    lua_pushnumber(L, self->GetDamping());

    return 1;
}

int Wrap_WeldJoint::GetReferenceAngle(lua_State* L)
{
    WeldJoint* self = Wrap_WeldJoint::CheckWeldJoint(L, 1);

    lua_pushnumber(L, self->GetReferenceAngle());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getDamping",        Wrap_WeldJoint::GetDamping        },
    { "getDampingRatio",   Wrap_WeldJoint::GetDampingRatio   },
    { "getFrequency",      Wrap_WeldJoint::GetFrequency      },
    { "getReferenceAngle", Wrap_WeldJoint::GetReferenceAngle },
    { "getStiffness",      Wrap_WeldJoint::GetStiffness      },
    { "setDamping",        Wrap_WeldJoint::SetDamping        },
    { "setDampingRatio",   Wrap_WeldJoint::SetDampingRatio   },
    { "setFrequency",      Wrap_WeldJoint::SetFrequency      },
    { "setStiffness",      Wrap_WeldJoint::SetStiffness      },
    { 0,                   0                                 }
};
// clang-format on

int Wrap_WeldJoint::Register(lua_State* L)
{
    return Luax::RegisterType(L, &WeldJoint::type, Wrap_Joint::functions, functions, nullptr);
}
