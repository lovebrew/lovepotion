#include <objects/joint/types/frictionjoint/frictionjoint.hpp>

#include <common/math.hpp>

#include <modules/physics/physics.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

using namespace love;

Type FrictionJoint::type("FrictionJoint", &Joint::type);

FrictionJoint::FrictionJoint(Body* a, Body* b, float x1, float y1, float x2, float y2,
                             bool collideConnected) :
    Joint(a, b),
    joint(nullptr)
{
    const auto first  = Physics::ScaleDown(b2Vec2(x1, y1));
    const auto second = Physics::ScaleDown(b2Vec2(x2, y2));

    b2FrictionJointDef definition {};
    definition.Initialize(a->body, b->body, first);
    definition.localAnchorB     = b->body->GetLocalPoint(second);
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
    const auto _torque = Physics::ScaleDown(Physics::ScaleDown(torque));
    this->joint->SetMaxTorque(_torque);
}

float FrictionJoint::GetMaxTorque() const
{
    return Physics::ScaleUp(Physics::ScaleUp(this->joint->GetMaxTorque()));
}
