#include "motorjoint/motorjoint.h"

#include "modules/physics/physics.h"

using namespace love;

love::Type MotorJoint::type("MotorJoint", &Joint::type);

MotorJoint::MotorJoint(Body* a, Body* b) : Joint(a, b), joint(NULL)
{
    b2MotorJointDef definition;
    definition.Initialize(a->body, b->body);

    this->joint = (b2MotorJoint*)this->CreateJoint(&definition);
}

MotorJoint::MotorJoint(Body* a, Body* b, float correctionFactor, bool collideConnected) :
    Joint(a, b),
    joint(NULL)
{
    b2MotorJointDef definition;
    definition.Initialize(a->body, b->body);
    definition.correctionFactor = correctionFactor;
    definition.collideConnected = collideConnected;

    this->joint = (b2MotorJoint*)this->CreateJoint(&definition);
}

MotorJoint::~MotorJoint()
{}

void MotorJoint::SetLinearOffset(float x, float y)
{
    b2Vec2 offset = Physics::ScaleDown(b2Vec2(x, y));
    this->joint->SetLinearOffset(offset);
}

int MotorJoint::GetLinearOffset(lua_State* L) const
{
    b2Vec2 offset = this->joint->GetLinearOffset();

    lua_pushnumber(L, Physics::ScaleUp(offset.x));
    lua_pushnumber(L, Physics::ScaleUp(offset.y));

    return 2;
}

void MotorJoint::SetAngularOffset(float offset)
{
    this->joint->SetAngularOffset(offset);
}

float MotorJoint::GetAngularOffset() const
{
    return this->joint->GetAngularOffset();
}

void MotorJoint::SetMaxForce(float force)
{
    this->joint->SetMaxForce(Physics::ScaleDown(force));
}

float MotorJoint::GetMaxForce() const
{
    return Physics::ScaleUp(this->joint->GetMaxForce());
}

void MotorJoint::SetMaxTorque(float torque)
{
    this->joint->SetMaxTorque(Physics::ScaleDown(Physics::ScaleDown(torque)));
}

float MotorJoint::GetMaxTorque() const
{
    return Physics::ScaleUp(Physics::ScaleUp(this->joint->GetMaxTorque()));
}

void MotorJoint::SetCorrectionFactor(float factor)
{
    this->joint->SetCorrectionFactor(factor);
}

float MotorJoint::GetCorrectionFactor() const
{
    return this->joint->GetCorrectionFactor();
}
