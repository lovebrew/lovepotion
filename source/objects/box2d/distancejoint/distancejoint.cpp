#include "distancejoint/distancejoint.h"

#include "modules/physics/physics.h"

using namespace love;

DistanceJoint::DistanceJoint(Body* a, Body* b, float x1, float y1, float x2, float y2,
                             bool collideConnected) :
    Joint(a, b),
    joint(NULL)
{}

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

void DistanceJoint::SetFrequency(float hz)
{
    float stiffness, damping;
    b2LinearStiffness(stiffness, damping, hz, this->GetDampingRatio(), this->joint->GetBodyA(),
                      this->joint->GetBodyB());

    this->joint->SetStiffness(stiffness);
}

float DistanceJoint::GetFrequency() const
{
    float frequency, ratio;
    Physics::b2LinearFrequency(frequency, ratio, this->joint->GetStiffness(),
                               this->joint->GetDamping(), this->joint->GetBodyA(),
                               this->joint->GetBodyB());

    return frequency;
}

void DistanceJoint::SetDampingRatio(float ratio)
{
    float stiffness, damping;
    b2LinearStiffness(stiffness, damping, this->GetFrequency(), ratio, this->joint->GetBodyA(),
                      this->joint->GetBodyB());

    this->joint->SetDamping(damping);
}

float DistanceJoint::GetDampingRatio() const
{
    float frequency, ratio;
    Physics::b2LinearFrequency(frequency, ratio, this->joint->GetStiffness(),
                               this->joint->GetDamping(), this->joint->GetBodyA(),
                               this->joint->GetBodyB());

    return ratio;
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
