#include "weldjoint/weldjoint.h"

#include "modules/physics/physics.h"

using namespace love;

love::Type WeldJoint::type("WeldJoint", &Joint::type);

WeldJoint::WeldJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                     bool collideConnected) :
    Joint(a, b),
    joint(NULL)
{
    b2WeldJointDef definition;
    this->Initialize(definition, a, b, xA, yA, xB, yB, collideConnected);

    this->joint = (b2WeldJoint*)this->CreateJoint(&definition);
}

WeldJoint::WeldJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                     bool collideConnected, float referenceAngle) :
    Joint(a, b),
    joint(NULL)
{
    b2WeldJointDef definition;
    this->Initialize(definition, a, b, xA, yA, xB, yB, collideConnected);
    definition.referenceAngle = referenceAngle;

    this->joint = (b2WeldJoint*)this->CreateJoint(&definition);
}

void WeldJoint::Initialize(b2WeldJointDef& definition, Body* a, Body* b, float xA, float yA,
                           float xB, float yB, bool collideConnected)
{
    definition.Initialize(a->body, b->body, Physics::ScaleDown(b2Vec2(xA, yA)));
    definition.localAnchorB     = b->body->GetLocalPoint(Physics::ScaleDown(b2Vec2(xB, yB)));
    definition.collideConnected = collideConnected;
}

WeldJoint::~WeldJoint()
{}

void WeldJoint::SetFrequency(float hz)
{
    float stiffness, damping;
    b2LinearStiffness(stiffness, damping, hz, this->GetDampingRatio(), joint->GetBodyA(),
                      this->joint->GetBodyB());

    this->joint->SetStiffness(stiffness);
}

float WeldJoint::GetFrequency() const
{
    float frequency, ratio;
    Physics::b2LinearFrequency(frequency, ratio, this->joint->GetStiffness(),
                               this->joint->GetDamping(), this->joint->GetBodyA(),
                               this->joint->GetBodyB());

    return frequency;
}

void WeldJoint::SetDampingRatio(float ratio)
{
    float stiffness, damping;
    b2LinearStiffness(stiffness, damping, this->GetFrequency(), ratio, this->joint->GetBodyA(),
                      this->joint->GetBodyB());

    this->joint->SetDamping(damping);
}

float WeldJoint::GetDampingRatio() const
{
    float frequency, ratio;
    Physics::b2LinearFrequency(frequency, ratio, this->joint->GetStiffness(),
                               this->joint->GetDamping(), this->joint->GetBodyA(),
                               this->joint->GetBodyB());

    return ratio;
}

void WeldJoint::SetStiffness(float stiffness)
{
    this->joint->SetStiffness(stiffness);
}

float WeldJoint::GetStiffness() const
{
    return this->joint->GetStiffness();
}

void WeldJoint::SetDamping(float damping)
{
    this->joint->SetDamping(damping);
}

float WeldJoint::GetDamping() const
{
    return this->joint->GetDamping();
}

float WeldJoint::GetReferenceAngle() const
{
    return this->joint->GetReferenceAngle();
}
