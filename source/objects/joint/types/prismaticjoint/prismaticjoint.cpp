#include <objects/joint/types/prismaticjoint/prismaticjoint.hpp>

#include <modules/physics/physics.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

using namespace love;

Type PrismaticJoint::type("PrismaticJoint", &Joint::type);

PrismaticJoint::PrismaticJoint(Body* bodyA, Body* bodyB, float xA, float yA, float xB, float yB,
                               float aX, float aY, bool collideConnected) :
    Joint(bodyA, bodyB),
    joint(nullptr)
{
    b2PrismaticJointDef definition {};
    this->Init(definition, bodyA, bodyB, xA, yA, xB, yB, aX, aY, collideConnected);

    this->joint = (b2PrismaticJoint*)this->CreateJoint(&definition);
}

PrismaticJoint::PrismaticJoint(Body* bodyA, Body* bodyB, float xA, float yA, float xB, float yB,
                               float aX, float aY, bool collideConnected, float referenceAngle) :
    Joint(bodyA, bodyB),
    joint(nullptr)
{
    b2PrismaticJointDef definition {};
    this->Init(definition, bodyA, bodyB, xA, yA, xB, yB, aX, aY, collideConnected);
    definition.referenceAngle = referenceAngle;

    this->joint = (b2PrismaticJoint*)this->CreateJoint(&definition);
}

PrismaticJoint::~PrismaticJoint()
{}

void PrismaticJoint::Init(b2PrismaticJointDef& definition, Body* bodyA, Body* bodyB, float xA,
                          float yA, float xB, float yB, float aX, float aY, bool collideConnected)
{
    const auto anchor = Physics::ScaleDown(b2Vec2(xA, yA));
    const auto axis   = b2Vec2(aX, aY);

    definition.Initialize(bodyA->body, bodyB->body, anchor, axis);
    definition.localAnchorB     = bodyB->body->GetLocalPoint(Physics::ScaleDown(b2Vec2(xB, yB)));
    definition.lowerTranslation = 0.0f;
    definition.upperTranslation = 100.0f;
    definition.enableLimit      = true;
    definition.collideConnected = collideConnected;
}

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

void PrismaticJoint::SetMotorSpeed(float speed)
{
    this->joint->SetMotorSpeed(Physics::ScaleDown(speed));
}

float PrismaticJoint::GetMotorSpeed() const
{
    return Physics::ScaleUp(this->joint->GetMotorSpeed());
}

float PrismaticJoint::GetMotorForce(float inverseDelta) const
{
    return Physics::ScaleUp(this->joint->GetMotorForce(inverseDelta));
}

float PrismaticJoint::GetMaxMotorForce() const
{
    return Physics::ScaleUp(this->joint->GetMaxMotorForce());
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

void PrismaticJoint::SetLowerLimit(float limit)
{
    this->joint->SetLimits(Physics::ScaleDown(limit), this->joint->GetUpperLimit());
}

void PrismaticJoint::SetLimits(float lower, float upper)
{
    this->joint->SetLimits(Physics::ScaleDown(lower), Physics::ScaleDown(upper));
}

float PrismaticJoint::GetUpperLimit() const
{
    return Physics::ScaleUp(this->joint->GetUpperLimit());
}

float PrismaticJoint::GetLowerLimit() const
{
    return Physics::ScaleUp(this->joint->GetLowerLimit());
}

int PrismaticJoint::GetLimits(lua_State* L)
{
    lua_pushnumber(L, Physics::ScaleUp(this->joint->GetLowerLimit()));
    lua_pushnumber(L, Physics::ScaleUp(this->joint->GetUpperLimit()));

    return 2;
}

int PrismaticJoint::GetAxis(lua_State* L)
{
    auto axis = this->joint->GetLocalAxisA();
    this->GetBodyA()->GetWorldVector(axis.x, axis.y, axis.x, axis.y);

    lua_pushnumber(L, axis.x);
    lua_pushnumber(L, axis.y);

    return 2;
}

float PrismaticJoint::GetReferenceAngle() const
{
    return this->joint->GetReferenceAngle();
}
