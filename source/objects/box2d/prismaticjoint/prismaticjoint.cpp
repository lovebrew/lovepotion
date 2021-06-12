#include "prismaticjoint/prismaticjoint.h"

#include "modules/physics/physics.h"

using namespace love;

love::Type PrismaticJoint::type("PrismaticJoint", &Joint::type);

PrismaticJoint::PrismaticJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                               float xAnchor, float yAnchor, bool collideConnected) :
    Joint(a, b),
    joint(NULL)
{
    b2PrismaticJointDef definition;
    this->Initialize(definition, a, b, xA, yA, xB, yB, xAnchor, yAnchor, collideConnected);

    this->joint = (b2PrismaticJoint*)this->CreateJoint(&definition);
}

PrismaticJoint::PrismaticJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                               float xAnchor, float yAnchor, bool collideConnected,
                               float referenceAngle) :
    Joint(a, b),
    joint(NULL)
{
    b2PrismaticJointDef definition;
    this->Initialize(definition, a, b, xA, yA, xB, yB, xAnchor, yAnchor, collideConnected);
    definition.referenceAngle = referenceAngle;

    this->joint = (b2PrismaticJoint*)this->CreateJoint(&definition);
}

void PrismaticJoint::Initialize(b2PrismaticJointDef& definition, Body* a, Body* b, float xA,
                                float yA, float xB, float yB, float xAnchor, float yAnchor,
                                bool collideConnected)
{
    definition.Initialize(a->body, b->body, Physics::ScaleDown(b2Vec2(xA, yA)),
                          b2Vec2(xAnchor, yAnchor));

    definition.localAnchorB     = b->body->GetLocalPoint(Physics::ScaleDown(b2Vec2(xB, yB)));
    definition.lowerTranslation = 0.0f;
    definition.upperTranslation = 100.0f;
    definition.enableLimit      = true;
    definition.collideConnected = collideConnected;
}

PrismaticJoint::~PrismaticJoint()
{}

float PrismaticJoint::GetJointTranslation() const
{
    return Physics::ScaleUp(this->joint->GetJointTranslation());
}

float PrismaticJoint::GetJointSpeed() const
{
    return Physics::ScaleUp(this->joint->GetJointSpeed());
}

void PrismaticJoint::SetMotorEnabled(bool enable)
{
    this->joint->EnableMotor(enable);
}

bool PrismaticJoint::IsMotorEnabled() const
{
    return this->joint->IsMotorEnabled();
}

void PrismaticJoint::SetMaxMotorForce(float force)
{
    this->joint->SetMaxMotorForce(Physics::ScaleDown(force));
}

float PrismaticJoint::GetMaxMotorForce() const
{
    return Physics::ScaleUp(this->joint->GetMaxMotorForce());
}

void PrismaticJoint::SetMotorSpeed(float speed)
{
    this->joint->SetMotorSpeed(Physics::ScaleDown(speed));
}

float PrismaticJoint::GetMotorSpeed() const
{
    return Physics::ScaleUp(this->joint->GetMotorSpeed());
}

float PrismaticJoint::GetMotorForce(float invDt) const
{
    return Physics::ScaleUp(this->joint->GetMotorForce(invDt));
}

void PrismaticJoint::SetLimitsEnabled(bool enable)
{
    this->joint->EnableLimit(enable);
}

bool PrismaticJoint::AreLimitsEnabled() const
{
    return this->joint->IsLimitEnabled();
}

void PrismaticJoint::SetUpperLimit(float limit)
{
    this->joint->SetLimits(this->joint->GetLowerLimit(), Physics::ScaleDown(limit));
}

float PrismaticJoint::GetUpperLimit() const
{
    return Physics::ScaleUp(this->joint->GetUpperLimit());
}

void PrismaticJoint::SetLowerLimit(float limit)
{
    this->joint->SetLimits(Physics::ScaleDown(limit), this->joint->GetUpperLimit());
}

float PrismaticJoint::GetLowerLimit() const
{
    return Physics::ScaleUp(this->joint->GetLowerLimit());
}

void PrismaticJoint::SetLimits(float lower, float upper)
{
    this->joint->SetLimits(Physics::ScaleDown(lower), Physics::ScaleDown(upper));
}

int PrismaticJoint::GetLimits(lua_State* L)
{
    lua_pushnumber(L, this->GetLowerLimit());
    lua_pushnumber(L, this->GetUpperLimit());

    return 2;
}

int PrismaticJoint::GetAxis(lua_State* L)
{
    b2Vec2 axis = this->joint->GetLocalAxisA();
    this->GetBodyA()->GetWorldVector(axis.x, axis.y, axis.x, axis.y);

    lua_pushnumber(L, axis.x);
    lua_pushnumber(L, axis.y);

    return 2;
}

float PrismaticJoint::GetReferenceAngle() const
{
    return this->joint->GetReferenceAngle();
}
