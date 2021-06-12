#include "revolutejoint/revolutejoint.h"
#include "modules/physics/physics.h"

using namespace love;

love::Type RevoluteJoint::type("RevoluteJoint", &Joint::type);

RevoluteJoint::RevoluteJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                             bool collideConnected) :
    Joint(a, b),
    joint(NULL)
{
    b2RevoluteJointDef definition;
    this->Initialize(definition, a, b, xA, yA, xB, yB, collideConnected);

    this->joint = (b2RevoluteJoint*)this->CreateJoint(&definition);
}

RevoluteJoint::RevoluteJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                             bool collideConnected, float referenceAngle) :
    Joint(a, b),
    joint(NULL)
{
    b2RevoluteJointDef definition;
    this->Initialize(definition, a, b, xA, yA, xB, yB, collideConnected);
    definition.referenceAngle = referenceAngle;

    this->joint = (b2RevoluteJoint*)this->CreateJoint(&definition);
}

void RevoluteJoint::Initialize(b2RevoluteJointDef& definition, Body* a, Body* b, float xA, float yA,
                               float xB, float yB, bool collideConnected)
{
    definition.Initialize(a->body, b->body, Physics::ScaleDown(b2Vec2(xA, yA)));
    definition.localAnchorB     = b->body->GetLocalPoint(Physics::ScaleDown(b2Vec2(xB, yB)));
    definition.collideConnected = collideConnected;
}

RevoluteJoint::~RevoluteJoint()
{}

float RevoluteJoint::GetJointAngle() const
{
    return this->joint->GetJointAngle();
}

float RevoluteJoint::GetJointSpeed() const
{
    return this->joint->GetJointSpeed();
}

void RevoluteJoint::SetMotorEnabled(bool enable)
{
    this->joint->EnableMotor(enable);
}

bool RevoluteJoint::IsMotorEnabled() const
{
    return this->joint->IsMotorEnabled();
}

void RevoluteJoint::SetMaxMotorTorque(float torque)
{
    this->joint->SetMaxMotorTorque(Physics::ScaleDown(Physics::ScaleDown(torque)));
}

void RevoluteJoint::SetMotorSpeed(float speed)
{
    this->joint->SetMotorSpeed(speed);
}

float RevoluteJoint::GetMotorSpeed() const
{
    return this->joint->GetMotorSpeed();
}

float RevoluteJoint::GetMotorTorque(float invDt) const
{
    return Physics::ScaleUp(Physics::ScaleUp(this->joint->GetMotorTorque(invDt)));
}

float RevoluteJoint::GetMaxMotorTorque() const
{
    return Physics::ScaleUp(Physics::ScaleUp(this->joint->GetMaxMotorTorque()));
}

void RevoluteJoint::SetLimitsEnabled(bool enable)
{
    this->joint->EnableLimit(enable);
}

bool RevoluteJoint::AreLimitsEnabled() const
{
    return this->joint->IsLimitEnabled();
}

void RevoluteJoint::SetUpperLimit(float limit)
{
    this->joint->SetLimits(this->joint->GetLowerLimit(), limit);
}

float RevoluteJoint::GetUpperLimit() const
{
    return this->joint->GetUpperLimit();
}

void RevoluteJoint::SetLowerLimit(float limit)
{
    this->joint->SetLimits(limit, this->joint->GetUpperLimit());
}

float RevoluteJoint::GetLowerLimit() const
{
    return this->joint->GetLowerLimit();
}

void RevoluteJoint::SetLimits(float lower, float upper)
{
    this->joint->SetLimits(lower, upper);
}

int RevoluteJoint::GetLimits(lua_State* L)
{
    lua_pushnumber(L, this->GetLowerLimit());
    lua_pushnumber(L, this->GetUpperLimit());

    return 2;
}

float RevoluteJoint::GetReferenceAngle() const
{
    return this->joint->GetReferenceAngle();
}
