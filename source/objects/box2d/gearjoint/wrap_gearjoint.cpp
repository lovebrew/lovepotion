#include "gearjoint/wrap_gearjoint.h"
#include "joint/wrap_joint.h"

using namespace love;

GearJoint* Wrap_GearJoint::CheckGearJoint(lua_State* L, int index)
{
    GearJoint* joint = Luax::CheckType<GearJoint>(L, 1);

    if (!joint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint!");

    return joint;
}

int Wrap_GearJoint::SetRatio(lua_State* L)
{
    GearJoint* self = Wrap_GearJoint::CheckGearJoint(L, 1);
    float ratio     = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { self->SetRatio(ratio); });

    return 0;
}

int Wrap_GearJoint::GetRatio(lua_State* L)
{
    GearJoint* self = Wrap_GearJoint::CheckGearJoint(L, 1);

    lua_pushnumber(L, self->GetRatio());

    return 1;
}

int Wrap_GearJoint::GetJoints(lua_State* L)
{
    GearJoint* self = Wrap_GearJoint::CheckGearJoint(L, 1);

    Joint* a = nullptr;
    Joint* b = nullptr;

    Luax::CatchException(L, [&]() {
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
    { "getJoints", Wrap_GearJoint::GetJoints },
    { "getRatio",  Wrap_GearJoint::GetRatio  },
    { "setRatio",  Wrap_GearJoint::SetRatio  },
    { 0,           0                         }
};
// clang-format on

int Wrap_GearJoint::Register(lua_State* L)
{
    return Luax::RegisterType(L, &GearJoint::type, Wrap_Joint::functions, functions, nullptr);
}
