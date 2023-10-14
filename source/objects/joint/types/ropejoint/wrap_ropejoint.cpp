#include <objects/joint/types/ropejoint/wrap_ropejoint.hpp>

using namespace love;

RopeJoint* Wrap_RopeJoint::CheckRopeJoint(lua_State* L, int index)
{
    auto* ropeJoint = luax::CheckType<RopeJoint>(L, index);

    if (!ropeJoint->IsValid())
        luaL_error(L, "Attempt to use destroyed RopeJoint.");

    return ropeJoint;
}

int Wrap_RopeJoint::GetMaxLength(lua_State* L)
{
    auto* self = Wrap_RopeJoint::CheckRopeJoint(L, 1);

    lua_pushnumber(L, self->GetMaxLength());

    return 1;
}

int Wrap_RopeJoint::SetMaxLength(lua_State* L)
{
    auto* self = Wrap_RopeJoint::CheckRopeJoint(L, 1);

    float maxLength = luaL_checknumber(L, 2);

    self->SetMaxLength(maxLength);

    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getMaxLength", Wrap_RopeJoint::GetMaxLength },
    { "setMaxLength", Wrap_RopeJoint::SetMaxLength }
};
// clang-format on

int Wrap_RopeJoint::Register(lua_State* L)
{
    return luax::RegisterType(L, &RopeJoint::type, Wrap_Joint::jointFunctions, functions);
}
