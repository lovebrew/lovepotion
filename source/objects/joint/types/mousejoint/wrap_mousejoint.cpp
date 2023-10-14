#include <objects/joint/types/mousejoint/wrap_mousejoint.hpp>

using namespace love;

MouseJoint* Wrap_MouseJoint::CheckMouseJoint(lua_State* L, int index)
{
    auto* mouseJoint = luax::CheckType<MouseJoint>(L, index);

    if (!mouseJoint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint.");

    return mouseJoint;
}

int Wrap_MouseJoint::SetTarget(lua_State* L)
{
    auto* self = Wrap_MouseJoint::CheckMouseJoint(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    self->SetTarget(x, y);

    return 0;
}

int Wrap_MouseJoint::GetTarget(lua_State* L)
{
    auto* self = Wrap_MouseJoint::CheckMouseJoint(L, 1);
    lua_remove(L, 1);

    return self->GetTarget(L);
}

int Wrap_MouseJoint::SetMaxForce(lua_State* L)
{
    auto* self = Wrap_MouseJoint::CheckMouseJoint(L, 1);

    float force = luaL_checknumber(L, 2);
    self->SetMaxForce(force);

    return 0;
}

int Wrap_MouseJoint::GetMaxForce(lua_State* L)
{
    auto* self = Wrap_MouseJoint::CheckMouseJoint(L, 1);

    lua_pushnumber(L, self->GetMaxForce());

    return 1;
}

int Wrap_MouseJoint::SetStiffness(lua_State* L)
{
    auto* self = Wrap_MouseJoint::CheckMouseJoint(L, 1);

    float stiffness = luaL_checknumber(L, 2);
    luax::CatchException(L, [&]() { self->SetStiffness(stiffness); });

    return 0;
}

int Wrap_MouseJoint::GetStiffness(lua_State* L)
{
    auto* self = Wrap_MouseJoint::CheckMouseJoint(L, 1);

    lua_pushnumber(L, self->GetStiffness());

    return 1;
}

int Wrap_MouseJoint::SetDamping(lua_State* L)
{
    auto* self = Wrap_MouseJoint::CheckMouseJoint(L, 1);

    float damping = luaL_checknumber(L, 2);
    self->SetDamping(damping);

    return 0;
}

int Wrap_MouseJoint::GetDamping(lua_State* L)
{
    auto* self = Wrap_MouseJoint::CheckMouseJoint(L, 1);

    lua_pushnumber(L, self->GetDamping());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "setTarget",    Wrap_MouseJoint::SetTarget    },
    { "getTarget",    Wrap_MouseJoint::GetTarget    },
    { "setMaxForce",  Wrap_MouseJoint::SetMaxForce  },
    { "getMaxForce",  Wrap_MouseJoint::GetMaxForce  },
    { "setStiffness", Wrap_MouseJoint::SetStiffness },
    { "getStiffness", Wrap_MouseJoint::GetStiffness },
    { "setDamping",   Wrap_MouseJoint::SetDamping   },
    { "getDamping",   Wrap_MouseJoint::GetDamping   }
};
// clang-format on

int Wrap_MouseJoint::Register(lua_State* L)
{
    return luax::RegisterType(L, &MouseJoint::type, Wrap_Joint::jointFunctions, functions);
}
