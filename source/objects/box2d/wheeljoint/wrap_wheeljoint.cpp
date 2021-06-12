#include "wheeljoint/wrap_wheeljoint.h"
#include "joint/wrap_joint.h"

using namespace love;

WheelJoint* Wrap_WheelJoint::CheckWheelJoint(lua_State* L, int index)
{
    WheelJoint* joint = Luax::CheckType<WheelJoint>(L, 1);

    if (!joint->IsValid())
        luaL_error(L, "Attempt to use a destroyed joint!");

    return joint;
}

int Wrap_WheelJoint::GetJointTranslation(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);

    lua_pushnumber(L, self->GetJointTranslation());

    return 1;
}

int Wrap_WheelJoint::GetJointSpeed(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);

    lua_pushnumber(L, self->GetJointSpeed());

    return 1;
}

int Wrap_WheelJoint::SetMotorEnabled(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);
    bool enabled     = Luax::CheckBoolean(L, 2);

    self->SetMotorEnabled(enabled);

    return 0;
}

int Wrap_WheelJoint::IsMotorEnabled(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);

    Luax::PushBoolean(L, self->IsMotorEnabled());

    return 1;
}

int Wrap_WheelJoint::SetMotorSpeed(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);
    float speed      = luaL_checknumber(L, 2);

    self->SetMotorSpeed(speed);

    return 0;
}

int Wrap_WheelJoint::GetMotorSpeed(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);

    lua_pushnumber(L, self->GetMotorSpeed());

    return 1;
}

int Wrap_WheelJoint::SetMaxMotorTorque(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);
    float torque     = luaL_checknumber(L, 2);

    self->SetMaxMotorTorque(torque);

    return 0;
}

int Wrap_WheelJoint::GetMaxMotorTorque(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);

    lua_pushnumber(L, self->GetMaxMotorTorque());

    return 1;
}

int Wrap_WheelJoint::GetMotorTorque(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);
    float invDt      = luaL_checknumber(L, 2);

    lua_pushnumber(L, self->GetMotorTorque(invDt));

    return 1;
}

int Wrap_WheelJoint::SetFrequency(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);
    float frequency  = luaL_checknumber(L, 2);

    self->SetFrequency(frequency);

    return 0;
}

int Wrap_WheelJoint::GetFrequency(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);

    lua_pushnumber(L, self->GetFrequency());

    return 1;
}

int Wrap_WheelJoint::SetDampingRatio(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);
    float ratio      = luaL_checknumber(L, 2);

    self->SetDampingRatio(ratio);

    return 0;
}

int Wrap_WheelJoint::GetDampingRatio(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);

    lua_pushnumber(L, self->GetDampingRatio());

    return 1;
}

int Wrap_WheelJoint::SetStiffness(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);
    float stiffness  = luaL_checknumber(L, 2);

    self->SetStiffness(stiffness);

    return 0;
}

int Wrap_WheelJoint::GetStiffness(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);

    lua_pushnumber(L, self->GetStiffness());

    return 1;
}

int Wrap_WheelJoint::SetDamping(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);
    float damping    = luaL_checknumber(L, 2);

    self->SetDamping(damping);

    return 0;
}

int Wrap_WheelJoint::GetDamping(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);

    lua_pushnumber(L, self->GetDamping());

    return 1;
}

int Wrap_WheelJoint::GetAxis(lua_State* L)
{
    WheelJoint* self = Wrap_WheelJoint::CheckWheelJoint(L, 1);
    lua_remove(L, 1);

    return self->GetAxis(L);
}

int Wrap_WheelJoint::Register(lua_State* L)
{
    luaL_Reg funcs[] = { { "getJointTranslation", GetJointTranslation },
                         { "getJointSpeed", GetJointSpeed },
                         { "setMotorEnabled", SetMotorEnabled },
                         { "isMotorEnabled", IsMotorEnabled },
                         { "setMotorSpeed", SetMotorSpeed },
                         { "getMotorSpeed", GetMotorSpeed },
                         { "setMaxMotorTorque", SetMaxMotorTorque },
                         { "getMaxMotorTorque", GetMaxMotorTorque },
                         { "getMotorTorque", GetMotorTorque },
                         { "setSpringFrequency", SetFrequency },
                         { "getSpringFrequency", GetFrequency },
                         { "setSpringDampingRatio", SetDampingRatio },
                         { "getSpringDampingRatio", GetDampingRatio },
                         { "setSpringStiffness", SetStiffness },
                         { "getSpringStiffness", GetStiffness },
                         { "setSpringDamping", SetDamping },
                         { "getSpringDamping", GetDamping },
                         { "setFrequency", SetFrequency },
                         { "getFrequency", GetFrequency },
                         { "setDampingRatio", SetDampingRatio },
                         { "getDampingRatio", GetDampingRatio },
                         { "setStiffness", SetStiffness },
                         { "getStiffness", GetStiffness },
                         { "setDamping", SetDamping },
                         { "getDamping", GetDamping },
                         { "getAxis", GetAxis },
                         { 0, 0 } };

    return Luax::RegisterType(L, &WheelJoint::type, Wrap_Joint::functions, funcs, nullptr);
}
