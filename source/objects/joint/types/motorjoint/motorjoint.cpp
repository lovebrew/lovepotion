#include <objects/joint/types/motorjoint/motorjoint.hpp>

#include <modules/physics/physics.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

using namespace love;

Type MotorJoint::type("MotorJoint", &Joint::type);

MotorJoint::MotorJoint(Body* a, Body* b) : Joint(a, b), joint(nullptr)
{
    b2MotorJointDef definition {};
    definition.Initialize(a->body, b->body);

    this->joint = (b2MotorJoint*)this->CreateJoint(&definition);
}

MotorJoint::MotorJoint(Body* a, Body* b, float correctionFactor, bool collideConnected) :
    Joint(a, b),
    joint(nullptr)
{
    b2MotorJointDef definition {};
    definition.Initialize(a->body, b->body);
    definition.correctionFactor = correctionFactor;
    definition.collideConnected = collideConnected;

    this->joint = (b2MotorJoint*)this->CreateJoint(&definition);
}

MotorJoint::~MotorJoint()
{}

void MotorJoint::SetLinearOffset(float x, float y)
{
    this->joint->SetLinearOffset(Physics::ScaleDown(b2Vec2(x, y)));
}

int MotorJoint::GetLinearOffset(lua_State* L) const
{
    const auto offsetX = Physics::ScaleUp(this->joint->GetLinearOffset().x);
    const auto offsetY = Physics::ScaleUp(this->joint->GetLinearOffset().y);

    lua_pushnumber(L, offsetX);
    lua_pushnumber(L, offsetY);

    return 2;
}

void MotorJoint::SetAngularOffset(float angularOffset)
{
    this->joint->SetAngularOffset(angularOffset);
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
    const auto _torque = Physics::ScaleDown(Physics::ScaleDown(torque));
    this->joint->SetMaxTorque(_torque);
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
