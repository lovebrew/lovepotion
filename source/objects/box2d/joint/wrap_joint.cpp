#include "objects/box2d/joint/wrap_joint.h"

#include "body/body.h"

#include "distancejoint/distancejoint.h"
#include "frictionjoint/frictionjoint.h"
#include "gearjoint/gearjoint.h"
#include "motorjoint/motorjoint.h"
#include "mousejoint/mousejoint.h"
#include "prismaticjoint/prismaticjoint.h"
#include "pulleyjoint/pulleyjoint.h"
#include "revolutejoint/revolutejoint.h"
#include "ropejoint/ropejoint.h"
#include "weldjoint/weldjoint.h"

using namespace love;

void Wrap_Joint::PushJoint(lua_State* L, Joint* joint)
{
    if (joint == nullptr)
        return lua_pushnil(L);

    switch (joint->GetType())
    {
        case Joint::JOINT_DISTANCE:
            return Luax::PushType(L, DistanceJoint::type, joint);
        case Joint::JOINT_REVOLUTE:
            return Luax::PushType(L, RevoluteJoint::type, joint);
        case Joint::JOINT_PRISMATIC:
            return Luax::PushType(L, PrismaticJoint::type, joint);
        case Joint::JOINT_MOUSE:
            return Luax::PushType(L, MouseJoint::type, joint);
        case Joint::JOINT_PULLEY:
            return Luax::PushType(L, PulleyJoint::type, joint);
        case Joint::JOINT_GEAR:
            return Luax::PushType(L, GearJoint::type, joint);
        case Joint::JOINT_FRICTION:
            return Luax::PushType(L, FrictionJoint::type, joint);
        case Joint::JOINT_WELD:
            return Luax::PushType(L, WeldJoint::type, joint);
        // case Joint::JOINT_WHEEL:
        //     return Luax::PushType(L, WheelJoint::type, joint);
        case Joint::JOINT_ROPE:
            return Luax::PushType(L, RopeJoint::type, joint);
        case Joint::JOINT_MOTOR:
            return Luax::PushType(L, MotorJoint::type, joint);
        default:
            return lua_pushnil(L);
    }
}

Joint* Wrap_Joint::CheckJoint(lua_State* L, int index)
{
    Joint* joint = Luax::CheckType<Joint>(L, index);

    if (!joint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint!");

    return joint;
}

int Wrap_Joint::GetType(lua_State* L)
{
    Joint* self      = Wrap_Joint::CheckJoint(L, 1);
    const char* type = "";

    Joint::GetConstant(self->GetType(), type);

    lua_pushstring(L, type);

    return 1;
}

int Wrap_Joint::GetBodies(lua_State* L)
{
    Joint* self = Wrap_Joint::CheckJoint(L, 1);

    Body* a = nullptr;
    Body* b = nullptr;

    Luax::CatchException(L, [&]() {
        a = self->GetBodyA();
        b = self->GetBodyB();
    });

    Luax::PushType(L, a);
    Luax::PushType(L, b);

    return 2;
}

int Wrap_Joint::GetAnchors(lua_State* L)
{
    Joint* self = Wrap_Joint::CheckJoint(L, 1);
    lua_remove(L, 1);

    return self->GetAnchors(L);
}

int Wrap_Joint::GetReactionForce(lua_State* L)
{
    Joint* self = Wrap_Joint::CheckJoint(L, 1);
    lua_remove(L, 1);

    return self->GetReactionForce(L);
}

int Wrap_Joint::GetReactionTorque(lua_State* L)
{
    Joint* self = Wrap_Joint::CheckJoint(L, 1);
    float dt    = luaL_checknumber(L, 2);

    lua_pushnumber(L, self->GetReactionTorque(dt));

    return 1;
}

int Wrap_Joint::GetCollideConnected(lua_State* L)
{
    Joint* self = Wrap_Joint::CheckJoint(L, 1);

    Luax::PushBoolean(L, self->GetCollideConnected());

    return 1;
}

int Wrap_Joint::SetUserdata(lua_State* L)
{
    Joint* self = Wrap_Joint::CheckJoint(L, 1);
    lua_remove(L, 1);

    return self->SetUserdata(L);
}

int Wrap_Joint::GetUserdata(lua_State* L)
{
    Joint* self = Wrap_Joint::CheckJoint(L, 1);
    lua_remove(L, 1);

    return self->GetUserdata(L);
}

int Wrap_Joint::Destroy(lua_State* L)
{
    Joint* self = Wrap_Joint::CheckJoint(L, 1);

    Luax::CatchException(L, [&]() { self->DestroyJoint(); });

    return 0;
}

int Wrap_Joint::IsDestroyed(lua_State* L)
{
    Joint* joint = Luax::CheckType<Joint>(L, 1);

    Luax::PushBoolean(L, !joint->IsValid());

    return 1;
}

// clang-format off
const luaL_Reg Wrap_Joint::functions[11] = {
    { "getType", Wrap_Joint::GetType },
    { "getBodies", Wrap_Joint::GetBodies },
    { "getAnchors", Wrap_Joint::GetAnchors },
    { "getReactionForce", Wrap_Joint::GetReactionForce },
    { "getReactionTorque", Wrap_Joint::GetReactionTorque },
    { "getCollideConnected", Wrap_Joint::GetCollideConnected },
    { "setUserData", Wrap_Joint::SetUserdata },
    { "getUserData", Wrap_Joint::GetUserdata },
    { "destroy", Wrap_Joint::Destroy },
    { "isDestroyed", Wrap_Joint::IsDestroyed },
    { 0, 0 }
};
// clang-format on

int Wrap_Joint::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Joint::type, Wrap_Joint::functions, nullptr);
}
