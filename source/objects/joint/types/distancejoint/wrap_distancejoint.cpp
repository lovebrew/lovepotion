#include <objects/joint/types/distancejoint/wrap_distancejoint.hpp>

using namespace love;

DistanceJoint* Wrap_DistanceJoint::CheckDistanceJoint(lua_State* L, int index)
{
    auto* distanceJoint = luax::CheckType<DistanceJoint>(L, index);

    if (!distanceJoint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint.");

    return distanceJoint;
}

int Wrap_DistanceJoint::SetLength(lua_State* L)
{
    auto* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);

    float length = luaL_checknumber(L, 2);
    self->SetLength(length);

    return 0;
}

int Wrap_DistanceJoint::GetLength(lua_State* L)
{
    auto* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);

    lua_pushnumber(L, self->GetLength());

    return 1;
}

int Wrap_DistanceJoint::SetStiffness(lua_State* L)
{
    auto* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);

    float stiffness = luaL_checknumber(L, 2);
    self->SetStiffness(stiffness);

    return 0;
}

int Wrap_DistanceJoint::GetStiffness(lua_State* L)
{
    auto* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);

    lua_pushnumber(L, self->GetStiffness());

    return 1;
}

int Wrap_DistanceJoint::SetDamping(lua_State* L)
{
    auto* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);

    float damping = luaL_checknumber(L, 2);
    self->SetDamping(damping);

    return 0;
}

int Wrap_DistanceJoint::GetDamping(lua_State* L)
{
    auto* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);

    lua_pushnumber(L, self->GetDamping());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "setLength",    Wrap_DistanceJoint::SetLength    },
    { "getLength",    Wrap_DistanceJoint::GetLength    },
    { "setStiffness", Wrap_DistanceJoint::SetStiffness },
    { "getStiffness", Wrap_DistanceJoint::GetStiffness },
    { "setDamping",   Wrap_DistanceJoint::SetDamping   },
    { "getDamping",   Wrap_DistanceJoint::GetDamping   }
};
// clang-format on

int Wrap_DistanceJoint::Register(lua_State* L)
{
    return luax::RegisterType(L, &DistanceJoint::type, Wrap_Joint::jointFunctions, functions);
}
