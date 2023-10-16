#include <objects/joint/joint.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

#include <modules/physics/physics.hpp>

#include <bitset>

using namespace love;

Type Joint::type("Joint", &Object::type);

Joint::Joint(Body* body) : world(body->world), bodyA(body), bodyB(nullptr)
{}

Joint::Joint(Body* a, Body* b) : world(a->world), bodyA(a), bodyB(b)
{}

Joint::~Joint()
{
    if (this->reference)
        delete this->reference;
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
    auto* first = this->joint->GetBodyA();

    if (first == nullptr)
        return nullptr;

    auto* body = (Body*)first->GetUserData().pointer;

    if (body == nullptr)
        throw love::Exception("A body has escaped Memoizer!");

    return body;
}

Body* Joint::GetBodyB() const
{
    auto* second = this->joint->GetBodyB();

    if (second == nullptr)
        return nullptr;

    auto* body = (Body*)second->GetUserData().pointer;

    if (body == nullptr)
        throw love::Exception("A body has escaped Memoizer!");

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
    float delta       = luaL_checknumber(L, 1);
    const auto vector = Physics::ScaleUp(this->joint->GetReactionForce(delta));

    lua_pushnumber(L, vector.x);
    lua_pushnumber(L, vector.y);

    return 2;
}

float Joint::GetReactionTorque(float delta)
{
    return Physics::ScaleUp(Physics::ScaleUp(this->joint->GetReactionTorque(delta)));
}

b2Joint* Joint::CreateJoint(b2JointDef* def)
{
    def->userData.pointer = (uintptr_t)this;
    this->joint           = this->world->world->CreateJoint(def);

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

    this->joint = nullptr;

    if (this->reference)
        this->reference->UnReference();

    this->Release();
}

bool Joint::IsEnabled() const
{
    return this->joint->IsEnabled();
}

bool Joint::GetCollideConnected() const
{
    return this->joint->GetCollideConnected();
}

int Joint::SetUserdata(lua_State* L)
{
    luax::AssertArgCount<1, 1>(L);

    if (!this->reference)
        this->reference = new Reference();

    this->reference->Create(L);
    return 0;
}

int Joint::GetUserdata(lua_State* L)
{
    if (this->reference != nullptr)
        this->reference->Push(L);
    else
        lua_pushnil(L);

    return 1;
}
