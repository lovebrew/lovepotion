#include <objects/joint/types/revolutejoint/revolutejoint.hpp>

#include <modules/physics/physics.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

using namespace love;

Type RevoluteJoint::type("RevoluteJoint", &Joint::type);

RevoluteJoint::RevoluteJoint(Body* bodyA, Body* bodyB, float xA, float yA, float xB, float yB,
                             bool collideConnected) :
    Joint(bodyA, bodyB),
    joint(nullptr)
{
    b2RevoluteJointDef definition {};
    this->Init(definition, bodyA, bodyB, xA, yA, xB, yB, collideConnected);

    this->joint = (b2RevoluteJoint*)this->CreateJoint(&definition);
}

RevoluteJoint::RevoluteJoint(Body* bodyA, Body* bodyB, float xA, float yA, float xB, float yB,
                             bool collideConnected, float referenceAngle) :
    Joint(bodyA, bodyB),
    joint(nullptr)
{
    b2RevoluteJointDef definition {};
    this->Init(definition, bodyA, bodyB, xA, yA, xB, yB, collideConnected);
    definition.referenceAngle = referenceAngle;

    this->joint = (b2RevoluteJoint*)this->CreateJoint(&definition);
}

RevoluteJoint::~RevoluteJoint()
{}

void RevoluteJoint::Init(b2RevoluteJointDef& definition, Body* bodyA, Body* bodyB, float xA,
                         float yA, float xB, float yB, bool collideConnected)
{
    const auto anchor = Physics::ScaleDown(b2Vec2(xA, yA));

    definition.Initialize(bodyA->body, bodyB->body, anchor);
    definition.localAnchorB     = bodyB->body->GetLocalPoint(Physics::ScaleDown(b2Vec2(xB, yB)));
    definition.collideConnected = collideConnected;
}

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
    const auto _torque = Physics::ScaleDown(Physics::ScaleDown(torque));
    this->joint->SetMaxMotorTorque(_torque);
}

void RevoluteJoint::SetMotorSpeed(float speed)
{
    this->joint->SetMotorSpeed(speed);
}

float RevoluteJoint::GetMotorSpeed() const
{
    return this->joint->GetMotorSpeed();
}

float RevoluteJoint::GetMotorTorque(float delta) const
{
    float inverseDelta = 1.0f / delta;
    return Physics::ScaleUp(Physics::ScaleUp(this->joint->GetMotorTorque(inverseDelta)));
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

void RevoluteJoint::SetLowerLimit(float limit)
{
    this->joint->SetLimits(limit, this->joint->GetUpperLimit());
}

void RevoluteJoint::SetLimits(float lower, float upper)
{
    this->joint->SetLimits(lower, upper);
}

float RevoluteJoint::GetUpperLimit() const
{
    return this->joint->GetUpperLimit();
}

float RevoluteJoint::GetLowerLimit() const
{
    return this->joint->GetLowerLimit();
}

int RevoluteJoint::GetLimits(lua_State* L)
{
    lua_pushnumber(L, this->joint->GetLowerLimit());
    lua_pushnumber(L, this->joint->GetUpperLimit());
    return 2;
}

float RevoluteJoint::GetReferenceAngle() const
{
    return this->joint->GetReferenceAngle();
}
