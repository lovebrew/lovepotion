#include "distancejoint/wrap_distancejoint.h"
#include "joint/wrap_joint.h"

using namespace love;

DistanceJoint* Wrap_DistanceJoint::CheckDistanceJoint(lua_State* L, int index)
{
    DistanceJoint* joint = Luax::CheckType<DistanceJoint>(L, index);

    if (!joint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint!");

    return joint;
}

int Wrap_DistanceJoint::SetLength(lua_State* L)
{
    DistanceJoint* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);
    float length        = luaL_checknumber(L, 2);

    self->SetLength(length);

    return 0;
}

int Wrap_DistanceJoint::GetLength(lua_State* L)
{
    DistanceJoint* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);

    lua_pushnumber(L, self->GetLength());

    return 1;
}

int Wrap_DistanceJoint::SetFrequency(lua_State* L)
{
    DistanceJoint* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);
    float frequency     = luaL_checknumber(L, 2);

    self->SetFrequency(frequency);

    return 0;
}

int Wrap_DistanceJoint::GetFrequency(lua_State* L)
{
    DistanceJoint* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);

    lua_pushnumber(L, self->GetFrequency());

    return 1;
}

int Wrap_DistanceJoint::SetDampingRatio(lua_State* L)
{
    DistanceJoint* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);
    float ratio         = luaL_checknumber(L, 2);

    self->SetDampingRatio(ratio);

    return 0;
}

int Wrap_DistanceJoint::GetDampingRatio(lua_State* L)
{
    DistanceJoint* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);

    lua_pushnumber(L, self->GetDampingRatio());

    return 1;
}

int Wrap_DistanceJoint::SetStiffness(lua_State* L)
{
    DistanceJoint* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);
    float stiffness     = luaL_checknumber(L, 2);

    self->SetStiffness(stiffness);

    return 0;
}

int Wrap_DistanceJoint::GetStiffness(lua_State* L)
{
    DistanceJoint* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);

    lua_pushnumber(L, self->GetStiffness());

    return 1;
}

int Wrap_DistanceJoint::SetDamping(lua_State* L)
{
    DistanceJoint* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);
    float damping       = luaL_checknumber(L, 2);

    self->SetDamping(damping);

    return 0;
}

int Wrap_DistanceJoint::GetDamping(lua_State* L)
{
    DistanceJoint* self = Wrap_DistanceJoint::CheckDistanceJoint(L, 1);

    lua_pushnumber(L, self->GetDamping());

    return 1;
}

int Wrap_DistanceJoint::Register(lua_State* L)
{
    luaL_Reg funcs[] = { { "setLength", SetLength },
                         { "getLength", GetLength },
                         { "setFrequency", SetFrequency },
                         { "getFrequency", GetFrequency },
                         { "setDampingRatio", SetDampingRatio },
                         { "getDampingRatio", GetDampingRatio },
                         { "setStiffness", SetStiffness },
                         { "getStiffness", GetStiffness },
                         { "setDamping", SetDamping },
                         { "getDamping", GetDamping },
                         { 0, 0 } };

    return Luax::RegisterType(L, &DistanceJoint::type, Wrap_Joint::functions, funcs, nullptr);
}
