#include "ropejoint/ropejoint.h"

#include "modules/physics/physics.h"

using namespace love;

love::Type RopeJoint::type("RopeJoint", &Joint::type);

RopeJoint::RopeJoint(Body* a, Body* b, float x1, float y1, float x2, float y2, float maxLength,
                     bool collideConnected) :
    Joint(a, b),
    joint(NULL)
{
    b2DistanceJointDef definition;
    definition.bodyA = a->body;
    definition.bodyB = b->body;

    a->GetLocalPoint(x1, y1, x1, y1);
    b->GetLocalPoint(x2, y2, x2, y2);

    definition.localAnchorA.x = Physics::ScaleDown(x1);
    definition.localAnchorA.y = Physics::ScaleDown(y1);
    definition.localAnchorB.x = Physics::ScaleDown(x2);
    definition.localAnchorB.y = Physics::ScaleDown(y2);

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
