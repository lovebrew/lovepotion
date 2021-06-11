#include "pulleyjoint/pulleyjoint.h"

#include "modules/physics/physics.h"

using namespace love;

love::Type PulleyJoint::type("PulleyJoint", &Joint::type);

PulleyJoint::PulleyJoint(Body* a, Body* b, b2Vec2 groundAnchorA, b2Vec2 groundAnchorB,
                         b2Vec2 anchorA, b2Vec2 anchorB, float ratio, bool collideConnected) :
    Joint(a, b),
    joint(NULL)
{
    b2PulleyJointDef definition;
    definition.Initialize(a->body, b->body, Physics::ScaleDown(groundAnchorA),
                          Physics::ScaleDown(groundAnchorB), Physics::ScaleDown(anchorA),
                          Physics::ScaleDown(anchorB), ratio);
    definition.collideConnected = collideConnected;

    this->joint = (b2PulleyJoint*)this->CreateJoint(&definition);
}

PulleyJoint::~PulleyJoint()
{}

int PulleyJoint::GetGroundAnchors(lua_State* L)
{
    b2Vec2 groundA = Physics::ScaleUp(this->joint->GetGroundAnchorA());
    b2Vec2 groundB = Physics::ScaleUp(this->joint->GetGroundAnchorB());

    lua_pushnumber(L, groundA.x);
    lua_pushnumber(L, groundA.y);

    lua_pushnumber(L, groundB.x);
    lua_pushnumber(L, groundB.y);

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
