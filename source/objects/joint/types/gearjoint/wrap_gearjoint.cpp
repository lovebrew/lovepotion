#include <objects/joint/types/gearjoint/wrap_gearjoint.hpp>

using namespace love;

GearJoint* Wrap_GearJoint::CheckGearJoint(lua_State* L, int index)
{
    auto* gearJoint = luax::CheckType<GearJoint>(L, index);

    if (!gearJoint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint.");

    return gearJoint;
}

int Wrap_GearJoint::SetRatio(lua_State* L)
{
    auto* self = Wrap_GearJoint::CheckGearJoint(L, 1);

    float ratio = luaL_checknumber(L, 2);
    luax::CatchException(L, [&]() { self->SetRatio(ratio); });

    return 0;
}

int Wrap_GearJoint::GetRatio(lua_State* L)
{
    auto* self = Wrap_GearJoint::CheckGearJoint(L, 1);

    lua_pushnumber(L, self->GetRatio());

    return 1;
}

int Wrap_GearJoint::GetJoints(lua_State* L)
{
    auto* self = Wrap_GearJoint::CheckGearJoint(L, 1);

    Joint* a = nullptr;
    Joint* b = nullptr;

    luax::CatchException(L, [&]() {
        a = self->GetJointA();
        b = self->GetJointB();
    });

    Wrap_Joint::PushJoint(L, a);
    Wrap_Joint::PushJoint(L, b);

    return 2;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "setRatio",  Wrap_GearJoint::SetRatio  },
    { "getRatio",  Wrap_GearJoint::GetRatio  },
    { "getJoints", Wrap_GearJoint::GetJoints }
};
// clang-format on

int Wrap_GearJoint::Register(lua_State* L)
{
    return luax::RegisterType(L, &GearJoint::type, Wrap_Joint::jointFunctions, functions);
}
