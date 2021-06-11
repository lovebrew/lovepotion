#include "ropejoint/wrap_ropejoint.h"
#include "joint/wrap_joint.h"

using namespace love;

RopeJoint* Wrap_RopeJoint::CheckRopeJoint(lua_State* L, int index)
{
    RopeJoint* joint = Luax::CheckType<RopeJoint>(L, index);

    if (!joint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint!");

    return joint;
}

int Wrap_RopeJoint::GetMaxLength(lua_State* L)
{
    RopeJoint* self = Wrap_RopeJoint::CheckRopeJoint(L, 1);

    lua_pushnumber(L, self->GetMaxLength());

    return 1;
}

int Wrap_RopeJoint::SetMaxLength(lua_State* L)
{
    RopeJoint* self = Wrap_RopeJoint::CheckRopeJoint(L, 1);
    float maxLength = luaL_checknumber(L, 2);

    self->SetMaxLength(maxLength);

    return 0;
}

int Wrap_RopeJoint::Register(lua_State* L)
{
    luaL_Reg funcs[] = { { "getMaxLength", GetMaxLength },
                         { "setMaxLength", SetMaxLength },
                         { 0, 0 } };

    return Luax::RegisterType(L, &RopeJoint::type, Wrap_Joint::functions, funcs, nullptr);
}
