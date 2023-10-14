#include <objects/joint/wrap_joint.hpp>

#include <objects/body/body.hpp>

#include <objects/joint/types/distancejoint/distancejoint.hpp>
#include <objects/joint/types/frictionjoint/frictionjoint.hpp>
#include <objects/joint/types/gearjoint/gearjoint.hpp>
#include <objects/joint/types/motorjoint/motorjoint.hpp>
#include <objects/joint/types/mousejoint/mousejoint.hpp>
#include <objects/joint/types/prismaticjoint/prismaticjoint.hpp>
#include <objects/joint/types/pulleyjoint/pulleyjoint.hpp>
#include <objects/joint/types/revolutejoint/revolutejoint.hpp>
#include <objects/joint/types/ropejoint/ropejoint.hpp>
#include <objects/joint/types/weldjoint/weldjoint.hpp>
#include <objects/joint/types/wheeljoint/wheeljoint.hpp>

using namespace love;

// TODO: Finish this
void Wrap_Joint::PushJoint(lua_State* L, Joint* joint)
{
    if (joint == nullptr)
        return lua_pushnil(L);

    switch (joint->GetType())
    {
        case Joint::Type::JOINT_DISTANCE:
            return luax::PushType(L, DistanceJoint::type, joint);
        case Joint::Type::JOINT_FRICTION:
            return luax::PushType(L, FrictionJoint::type, joint);
        case Joint::Type::JOINT_GEAR:
            return luax::PushType(L, GearJoint::type, joint);
        case Joint::Type::JOINT_MOTOR:
            return luax::PushType(L, MotorJoint::type, joint);
        case Joint::Type::JOINT_MOUSE:
            return luax::PushType(L, MouseJoint::type, joint);
        case Joint::Type::JOINT_PRISMATIC:
            return luax::PushType(L, PrismaticJoint::type, joint);
        case Joint::Type::JOINT_PULLEY:
            return luax::PushType(L, PulleyJoint::type, joint);
        case Joint::Type::JOINT_REVOLUTE:
            return luax::PushType(L, RevoluteJoint::type, joint);
        case Joint::Type::JOINT_ROPE:
            return luax::PushType(L, RopeJoint::type, joint);
        case Joint::Type::JOINT_WELD:
            return luax::PushType(L, WeldJoint::type, joint);
        case Joint::Type::JOINT_WHEEL:
            return luax::PushType(L, WheelJoint::type, joint);
        default:
            return lua_pushnil(L);
    }
}

Joint* Wrap_Joint::CheckJoint(lua_State* L, int index)
{
    auto* joint = luax::CheckType<Joint>(L, index);

    if (!joint->IsValid())
        luaL_error(L, "Attempt to use destroyed joint.");

    return joint;
}

int Wrap_Joint::GetType(lua_State* L)
{
    auto* self = Wrap_Joint::CheckJoint(L, 1);

    auto type = Joint::jointTypes.ReverseFind(self->GetType());
    lua_pushstring(L, *type);

    return 1;
}

int Wrap_Joint::GetBodies(lua_State* L)
{
    auto* self = Wrap_Joint::CheckJoint(L, 1);

    Body* a = nullptr;
    Body* b = nullptr;

    luax::CatchException(L, [&]() {
        a = self->GetBodyA();
        b = self->GetBodyB();
    });

    luax::PushType(L, a);
    luax::PushType(L, b);

    return 2;
}

int Wrap_Joint::GetAnchors(lua_State* L)
{
    auto* self = Wrap_Joint::CheckJoint(L, 1);
    lua_remove(L, 1);

    return self->GetAnchors(L);
}

int Wrap_Joint::GetReactionForce(lua_State* L)
{
    auto* self = Wrap_Joint::CheckJoint(L, 1);
    lua_remove(L, 1);

    return self->GetReactionForce(L);
}

int Wrap_Joint::GetReactionTorque(lua_State* L)
{
    auto* self = Wrap_Joint::CheckJoint(L, 1);

    float delta = luaL_checknumber(L, 2);
    lua_pushnumber(L, self->GetReactionTorque(delta));

    return 1;
}

int Wrap_Joint::GetCollideConnected(lua_State* L)
{
    auto* self = Wrap_Joint::CheckJoint(L, 1);

    luax::PushBoolean(L, self->GetCollideConnected());

    return 1;
}

int Wrap_Joint::SetUserdata(lua_State* L)
{
    auto* self = Wrap_Joint::CheckJoint(L, 1);
    lua_remove(L, 1);

    return self->SetUserdata(L);
}

int Wrap_Joint::GetUserdata(lua_State* L)
{
    auto* self = Wrap_Joint::CheckJoint(L, 1);
    lua_remove(L, 1);

    return self->GetUserdata(L);
}

int Wrap_Joint::Destroy(lua_State* L)
{
    auto* self = Wrap_Joint::CheckJoint(L, 1);

    luax::CatchException(L, [&]() { self->DestroyJoint(); });

    return 0;
}

int Wrap_Joint::IsDestroyed(lua_State* L)
{
    auto* self = luax::CheckType<Joint>(L, 1);

    luax::PushBoolean(L, !self->IsValid());

    return 1;
}

// clang-format off
const luaL_Reg Wrap_Joint::jointFunctions[0x0A] =
{
    { "getType",             Wrap_Joint::GetType             },
    { "getBodies",           Wrap_Joint::GetBodies           },
    { "getAnchors",          Wrap_Joint::GetAnchors          },
    { "getReactionForce",    Wrap_Joint::GetReactionForce    },
    { "getReactionTorque",   Wrap_Joint::GetReactionTorque   },
    { "getCollideConnected", Wrap_Joint::GetCollideConnected },
    { "setUserData",         Wrap_Joint::SetUserdata         },
    { "getUserData",         Wrap_Joint::GetUserdata         },
    { "destroy",             Wrap_Joint::Destroy             },
    { "isDestroyed",         Wrap_Joint::IsDestroyed         },
};
// clang-format on

int Wrap_Joint::Register(lua_State* L)
{
    return luax::RegisterType(L, &Joint::type, Wrap_Joint::jointFunctions);
}
