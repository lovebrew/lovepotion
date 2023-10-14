#include <objects/joint/types/ropejoint/ropejoint.hpp>

#include <modules/physics/physics.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

using namespace love;

Type RopeJoint::type("RopeJoint", &Joint::type);

RopeJoint::RopeJoint(Body* bodyA, Body* bodyB, float x1, float y1, float x2, float y2,
                     float maxLength, bool collideConnected) :
    Joint(bodyA, bodyB),
    joint(nullptr)
{
    bodyA->GetLocalPoint(x1, y1, x1, y1);
    bodyB->GetLocalPoint(x2, y2, x2, y2);

    b2DistanceJointDef definition {};
    definition.bodyA            = bodyA->body;
    definition.bodyB            = bodyB->body;
    definition.localAnchorA     = Physics::ScaleDown(b2Vec2(x1, y1));
    definition.localAnchorB     = Physics::ScaleDown(b2Vec2(x2, y2));
    definition.maxLength        = Physics::ScaleDown(maxLength);
    definition.collideConnected = collideConnected;

    this->joint = (b2DistanceJoint*)this->CreateJoint(&definition);
}

RopeJoint::~RopeJoint()
{}

float RopeJoint::GetMaxLength() const
{
    return Physics::ScaleUp(this->joint->GetMaxLength());
}

void RopeJoint::SetMaxLength(float maxLength)
{
    this->joint->SetMaxLength(Physics::ScaleDown(maxLength));
}
