#include <objects/joint/types/pulleyjoint/pulleyjoint.hpp>

#include <modules/physics/physics.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

using namespace love;

Type PulleyJoint::type("PulleyJoint", &Joint::type);

PulleyJoint::PulleyJoint(Body* bodyA, Body* bodyB, b2Vec2 groundAnchorA, b2Vec2 groundAnchorB,
                         b2Vec2 anchorA, b2Vec2 anchorB, float ratio, bool collideConnected) :
    Joint(bodyA, bodyB),
    joint(nullptr)
{
    const auto _groundAnchorA = Physics::ScaleDown(groundAnchorA);
    const auto _groundAnchorB = Physics::ScaleDown(groundAnchorB);

    const auto _anchorA = Physics::ScaleDown(anchorA);
    const auto _anchorB = Physics::ScaleDown(anchorB);

    b2PulleyJointDef definition {};
    definition.Initialize(bodyA->body, bodyB->body, _groundAnchorA, _groundAnchorB, _anchorA,
                          _anchorB, ratio);
    definition.collideConnected = collideConnected;

    this->joint = (b2PulleyJoint*)this->CreateJoint(&definition);
}

PulleyJoint::~PulleyJoint()
{}

int PulleyJoint::GetGroundAnchors(lua_State* L)
{
    lua_pushnumber(L, Physics::ScaleUp(this->joint->GetGroundAnchorA().x));
    lua_pushnumber(L, Physics::ScaleUp(this->joint->GetGroundAnchorA().y));
    lua_pushnumber(L, Physics::ScaleUp(this->joint->GetGroundAnchorB().x));
    lua_pushnumber(L, Physics::ScaleUp(this->joint->GetGroundAnchorB().y));

    return 4;
}

float PulleyJoint::GetLengthA() const
{
    return Physics::ScaleUp(this->joint->GetLengthA());
}

float PulleyJoint::GetLengthB() const
{
    return Physics::ScaleUp(this->joint->GetLengthB());
}

float PulleyJoint::GetRatio() const
{
    return this->joint->GetRatio();
}
