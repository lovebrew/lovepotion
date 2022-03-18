#include "objects/box2d/joint/joint.h"

#include <bitset>

#include "body/body.h"
#include "common/bidirectionalmap.h"
#include "physics/physics.h"
#include "world/world.h"

using namespace love;

love::Type Joint::type("Joint", &Object::type);

Joint::Joint(Body* body) : world(body->world), userdata(nullptr), bodyA(body), bodyB(nullptr)
{
    this->userdata = new JointUserdata { .ref = nullptr };
}

Joint::Joint(Body* a, Body* b) : world(a->world), userdata(nullptr), bodyA(a), bodyB(b)
{
    this->userdata = new JointUserdata { .ref = nullptr };
}

Joint::~Joint()
{
    if (!this->userdata)
        return;

    if (this->userdata->ref)
        delete this->userdata->ref;

    delete this->userdata;
}

Joint::Type Joint::GetType() const
{
    switch (this->joint->GetType())
    {
        case e_revoluteJoint:
            return JOINT_REVOLUTE;
        case e_prismaticJoint:
            return JOINT_PRISMATIC;
        case e_distanceJoint:
            return JOINT_DISTANCE;
        case e_pulleyJoint:
            return JOINT_PULLEY;
        case e_mouseJoint:
            return JOINT_MOUSE;
        case e_gearJoint:
            return JOINT_GEAR;
        case e_frictionJoint:
            return JOINT_FRICTION;
        case e_weldJoint:
            return JOINT_WELD;
        case e_wheelJoint:
            return JOINT_WHEEL;
        case e_ropeJoint:
            return JOINT_ROPE;
        case e_motorJoint:
            return JOINT_MOTOR;
        default:
            return JOINT_INVALID;
    }
}

Body* Joint::GetBodyA() const
{
    b2Body* bBody = this->joint->GetBodyA();
    if (bBody == nullptr)
        return nullptr;

    Body* body = (Body*)this->world->FindObject(bBody);
    if (body == nullptr)
        throw love::Exception("A body has escaped Memorizer!");

    return body;
}

Body* Joint::GetBodyB() const
{
    b2Body* bBody = this->joint->GetBodyB();
    if (bBody == nullptr)
        return nullptr;

    Body* body = (Body*)this->world->FindObject(bBody);
    if (body == nullptr)
        throw love::Exception("A body has escaped Memorizer!");

    return body;
}

bool Joint::IsValid() const
{
    return this->joint != nullptr;
}

int Joint::GetAnchors(lua_State* L)
{
    lua_pushnumber(L, Physics::ScaleUp(this->joint->GetAnchorA().x));
    lua_pushnumber(L, Physics::ScaleUp(this->joint->GetAnchorA().y));
    lua_pushnumber(L, Physics::ScaleUp(this->joint->GetAnchorB().x));
    lua_pushnumber(L, Physics::ScaleUp(this->joint->GetAnchorB().y));

    return 4;
}

int Joint::GetReactionForce(lua_State* L)
{
    float dt = luaL_checknumber(L, 1);

    b2Vec2 vec = Physics::ScaleUp(this->joint->GetReactionForce(dt));

    lua_pushnumber(L, vec.x);
    lua_pushnumber(L, vec.y);

    return 2;
}

float Joint::GetReactionTorque(float dt)
{
    return Physics::ScaleUp(Physics::ScaleUp(this->joint->GetReactionTorque(dt)));
}

b2Joint* Joint::CreateJoint(b2JointDef* definition)
{
    definition->userData.pointer = (uintptr_t)this->userdata;
    this->joint                  = this->world->world->CreateJoint(definition);

    this->world->RegisterObject(this->joint, this);
    this->Retain();

    return this->joint;
}

void Joint::DestroyJoint(bool implicit)
{
    if (this->world->world->IsLocked())
    {
        this->Retain();
        this->world->destructJoints.push_back(this);

        return;
    }

    if (!implicit && this->joint != nullptr)
        this->world->world->DestroyJoint(this->joint);

    this->world->UnRegisterObject(this->joint);
    this->joint = NULL;

    if (this->userdata && this->userdata->ref)
        this->userdata->ref->UnRef();

    this->Release();
}

bool Joint::IsEnabled() const
{
    return this->joint->IsEnabled();
}

bool Joint::GetCollideConnected() const
{
    return this->GetCollideConnected();
}

int Joint::SetUserdata(lua_State* L)
{
    Luax::AssertArgc<1, 1>(L);

    if (this->userdata == nullptr)
    {
        this->userdata                     = new JointUserdata();
        this->joint->GetUserData().pointer = (uintptr_t)this->userdata;
    }

    if (!this->userdata->ref)
        this->userdata->ref = new Reference();

    this->userdata->ref->Ref(L);

    return 0;
}

int Joint::GetUserdata(lua_State* L)
{
    if (this->userdata != nullptr && this->userdata->ref != nullptr)
        this->userdata->ref->Push(L);
    else
        lua_pushnil(L);

    return 1;
}

// clang-format off
constexpr auto types = BidirectionalMap<>::Create(
    "distance",  Joint::JOINT_DISTANCE,
    "revolute",  Joint::JOINT_REVOLUTE,
    "prismatic", Joint::JOINT_PRISMATIC,
    "mouse",     Joint::JOINT_MOUSE,
    "pulley",    Joint::JOINT_PULLEY,
    "gear",      Joint::JOINT_GEAR,
    "friction",  Joint::JOINT_FRICTION,
    "weld",      Joint::JOINT_WELD,
    "wheel",     Joint::JOINT_WHEEL,
    "rope",      Joint::JOINT_ROPE,
    "motor",     Joint::JOINT_MOTOR
);
// clang-format on

bool Joint::GetConstant(const char* in, Joint::Type& out)
{
    return types.Find(in, out);
}

bool Joint::GetConstant(Joint::Type in, const char*& out)
{
    return types.ReverseFind(in, out);
}
