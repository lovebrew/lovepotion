#include "frictionjoint/frictionjoint.h"

#include "modules/physics/physics.h"

using namespace love;

love::Type FrictionJoint::type("FrictionJoint", &Joint::type);

FrictionJoint::FrictionJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                             bool collideConnected) :
    Joint(a, b),
    joint(NULL)
{
    b2FrictionJointDef definition;
    definition.Initialize(a->body, b->body, Physics::ScaleDown(b2Vec2(xA, yA)));
    definition.localAnchorB     = b->body->GetLocalPoint(Physics::ScaleDown(b2Vec2(xB, yB)));
    definition.collideConnected = collideConnected;

    this->joint = (b2FrictionJoint*)this->CreateJoint(&definition);
}

FrictionJoint::~FrictionJoint()
{}

void FrictionJoint::SetMaxForce(float force)
{
    this->joint->SetMaxForce(Physics::ScaleDown(force));
}

float FrictionJoint::GetMaxForce() const
{
    return Physics::ScaleUp(this->joint->GetMaxForce());
}

void FrictionJoint::SetMaxTorque(float torque)
{
    this->joint->SetMaxTorque(Physics::ScaleDown(Physics::ScaleDown(torque)));
}

float FrictionJoint::GetMaxTorque() const
{
    return Physics::ScaleUp(Physics::ScaleUp(this->joint->GetMaxTorque()));
}
