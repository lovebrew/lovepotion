#include <objects/joint/types/weldjoint/weldjoint.hpp>

#include <modules/physics/physics.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

using namespace love;

Type WeldJoint::type("WeldJoint", &Joint::type);

WeldJoint::WeldJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                     bool collideConnected) :
    Joint(a, b),
    joint(nullptr)
{
    b2WeldJointDef definition {};
    this->Init(definition, a, b, xA, yA, xB, yB, collideConnected);

    this->joint = (b2WeldJoint*)this->CreateJoint(&definition);
}

WeldJoint::WeldJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                     bool collideConnected, float referenceAngle) :
    Joint(a, b),
    joint(nullptr)
{
    b2WeldJointDef definition {};
    this->Init(definition, a, b, xA, yA, xB, yB, collideConnected);
    definition.referenceAngle = referenceAngle;

    this->joint = (b2WeldJoint*)this->CreateJoint(&definition);
}

WeldJoint::~WeldJoint()
{}

void WeldJoint::Init(b2WeldJointDef& definition, Body* a, Body* b, float xA, float yA, float xB,
                     float yB, bool collideConnected)
{
    definition.Initialize(a->body, b->body, Physics::ScaleDown(b2Vec2(xA, yA)));
    definition.localAnchorB     = b->body->GetLocalPoint(Physics::ScaleDown(b2Vec2(xB, yB)));
    definition.collideConnected = collideConnected;
}

void WeldJoint::SetStiffness(float k)
{
    this->joint->SetStiffness(k);
}

float WeldJoint::GetStiffness() const
{
    return this->joint->GetStiffness();
}

void WeldJoint::SetDamping(float ratio)
{
    this->joint->SetDamping(ratio);
}

float WeldJoint::GetDamping() const
{
    return this->joint->GetDamping();
}

float WeldJoint::GetReferenceAngle() const
{
    return this->joint->GetReferenceAngle();
}
