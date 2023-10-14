#include <objects/joint/types/wheeljoint/wrap_wheeljoint.hpp>

using namespace love;

WheelJoint* Wrap_WheelJoint::CheckWheelJoint(lua_State* L, int index)
{
    auto* wheelJoint = luax::CheckType<WheelJoint>(L, index);

    if (!wheelJoint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint.");

    return wheelJoint;
}

int Wrap_WheelJoint::GetJointTranslation(lua_State* L)
{
    auto* self = CheckWheelJoint(L, 1);

    lua_pushnumber(L, self->GetJointTranslation());

    return 1;
}

int Wrap_WheelJoint::GetJointSpeed(lua_State* L)
{
    auto* self = CheckWheelJoint(L, 1);

    lua_pushnumber(L, self->GetJointSpeed());

    return 1;
}

int Wrap_WheelJoint::SetMotorEnabled(lua_State* L)
{
    auto* self = CheckWheelJoint(L, 1);

    bool enable = luax::CheckBoolean(L, 2);
    self->SetMotorEnabled(enable);

    return 0;
}

int Wrap_WheelJoint::IsMotorEnabled(lua_State* L)
{
    auto* self = CheckWheelJoint(L, 1);

    luax::PushBoolean(L, self->IsMotorEnabled());

    return 1;
}

int Wrap_WheelJoint::SetMotorSpeed(lua_State* L)
{
    auto* self = CheckWheelJoint(L, 1);

    float speed = luaL_checknumber(L, 2);
    self->SetMotorSpeed(speed);

    return 0;
}

int Wrap_WheelJoint::GetMotorSpeed(lua_State* L)
{
    auto* self = CheckWheelJoint(L, 1);

    lua_pushnumber(L, self->GetMotorSpeed());

    return 1;
}

int Wrap_WheelJoint::SetMaxMotorTorque(lua_State* L)
{
    auto* self = CheckWheelJoint(L, 1);

    float torque = luaL_checknumber(L, 2);
    self->SetMaxMotorTorque(torque);

    return 0;
}

int Wrap_WheelJoint::GetMaxMotorTorque(lua_State* L)
{
    auto* self = CheckWheelJoint(L, 1);

    lua_pushnumber(L, self->GetMaxMotorTorque());

    return 1;
}

int Wrap_WheelJoint::GetMotorTorque(lua_State* L)
{
    auto* self = CheckWheelJoint(L, 1);

    float dt = luaL_checknumber(L, 2);
    lua_pushnumber(L, self->GetMotorTorque(dt));

    return 1;
}

int Wrap_WheelJoint::SetStiffness(lua_State* L)
{
    auto* self = CheckWheelJoint(L, 1);

    float stiffness = luaL_checknumber(L, 2);
    self->SetStiffness(stiffness);

    return 0;
}

int Wrap_WheelJoint::GetStiffness(lua_State* L)
{
    auto* self = CheckWheelJoint(L, 1);

    lua_pushnumber(L, self->GetStiffness());

    return 1;
}

int Wrap_WheelJoint::SetDamping(lua_State* L)
{
    auto* self = CheckWheelJoint(L, 1);

    float damping = luaL_checknumber(L, 2);
    self->SetDamping(damping);

    return 0;
}

int Wrap_WheelJoint::GetDamping(lua_State* L)
{
    auto* self = CheckWheelJoint(L, 1);

    lua_pushnumber(L, self->GetDamping());

    return 1;
}

int Wrap_WheelJoint::GetAxis(lua_State* L)
{
    auto* self = CheckWheelJoint(L, 1);
    lua_remove(L, 1);

    return self->GetAxis(L);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getJointTranslation", Wrap_WheelJoint::GetJointTranslation },
    { "getJointSpeed",       Wrap_WheelJoint::GetJointSpeed       },
    { "setMotorEnabled",     Wrap_WheelJoint::SetMotorEnabled     },
    { "isMotorEnabled",      Wrap_WheelJoint::IsMotorEnabled      },
    { "setMotorSpeed",       Wrap_WheelJoint::SetMotorSpeed       },
    { "getMotorSpeed",       Wrap_WheelJoint::GetMotorSpeed       },
    { "setMaxMotorTorque",   Wrap_WheelJoint::SetMaxMotorTorque   },
    { "getMaxMotorTorque",   Wrap_WheelJoint::GetMaxMotorTorque   },
    { "getMotorTorque",      Wrap_WheelJoint::GetMotorTorque      },
    { "setStiffness",        Wrap_WheelJoint::SetStiffness        },
    { "getStiffness",        Wrap_WheelJoint::GetStiffness        },
    { "setDamping",          Wrap_WheelJoint::SetDamping          },
    { "getDamping",          Wrap_WheelJoint::GetDamping          },
    { "getAxis",             Wrap_WheelJoint::GetAxis             }
};
// clang-format on

int Wrap_WheelJoint::Register(lua_State* L)
{
    return luax::RegisterType(L, &WheelJoint::type, Wrap_Joint::jointFunctions, functions);
}
