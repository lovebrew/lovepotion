#include <objects/joint/types/distancejoint/distancejoint.hpp>

#include <modules/physics/physics.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

using namespace love;

Type DistanceJoint::type("DistanceJoint", &Joint::type);

DistanceJoint::DistanceJoint(Body* a, Body* b, float x1, float y1, float x2, float y2,
                             bool collideConnected) :
    Joint(a, b),
    joint(nullptr)
{
    const auto first  = Physics::ScaleDown(b2Vec2(x1, y1));
    const auto second = Physics::ScaleDown(b2Vec2(x2, y2));

    b2DistanceJointDef definition {};
    definition.Initialize(a->body, b->body, first, second);
    definition.collideConnected = collideConnected;

    this->joint = (b2DistanceJoint*)this->CreateJoint(&definition);
}

DistanceJoint::~DistanceJoint()
{}

void DistanceJoint::SetLength(float length)
{
    this->joint->SetLength(Physics::ScaleDown(length));
}

float DistanceJoint::GetLength() const
{
    return Physics::ScaleUp(this->joint->GetLength());
}

void DistanceJoint::SetStiffness(float k)
{
    this->joint->SetStiffness(k);
}

float DistanceJoint::GetStiffness() const
{
    return this->joint->GetStiffness();
}

void DistanceJoint::SetDamping(float d)
{
    this->joint->SetDamping(d);
}

float DistanceJoint::GetDamping() const
{
    return this->joint->GetDamping();
}
