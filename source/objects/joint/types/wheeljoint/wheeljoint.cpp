#include <objects/joint/types/wheeljoint/wheeljoint.hpp>

#include <modules/physics/physics.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

using namespace love;

Type WheelJoint::type("WheelJoint", &Joint::type);

WheelJoint::WheelJoint(Body* a, Body* b, float xA, float yA, float xB, float yB, float aX, float aY,
                       bool collideConnected) :
    Joint(a, b),
    joint(nullptr)
{
    b2WheelJointDef definition {};
    definition.Initialize(a->body, b->body, Physics::ScaleDown(b2Vec2(xA, yA)), b2Vec2(aX, aY));
    definition.localAnchorB     = b->body->GetLocalPoint(Physics::ScaleDown(b2Vec2(xB, yB)));
    definition.collideConnected = collideConnected;

    this->joint = (b2WheelJoint*)this->CreateJoint(&definition);
}

WheelJoint::~WheelJoint()
{}

float WheelJoint::GetJointTranslation() const
{
    return Physics::ScaleUp(this->joint->GetJointTranslation());
}

float WheelJoint::GetJointSpeed() const
{
    return Physics::ScaleUp(this->joint->GetJointLinearSpeed());
}

void WheelJoint::SetMotorEnabled(bool enable)
{
    this->joint->EnableMotor(enable);
}

bool WheelJoint::IsMotorEnabled() const
{
    return this->joint->IsMotorEnabled();
}

void WheelJoint::SetMotorSpeed(float speed)
{
    this->joint->SetMotorSpeed(speed);
}

float WheelJoint::GetMotorSpeed() const
{
    return this->joint->GetMotorSpeed();
}

void WheelJoint::SetMaxMotorTorque(float torque)
{
    const auto _torque = Physics::ScaleDown(Physics::ScaleDown(torque));
    this->joint->SetMaxMotorTorque(_torque);
}

float WheelJoint::GetMaxMotorTorque() const
{
    return Physics::ScaleUp(Physics::ScaleUp(this->joint->GetMaxMotorTorque()));
}

float WheelJoint::GetMotorTorque(float dt) const
{
    float inverseDelta = 1.0f / dt;
    return Physics::ScaleUp(Physics::ScaleUp(this->joint->GetMotorTorque(inverseDelta)));
}

void WheelJoint::SetStiffness(float stiffness)
{
    this->joint->SetStiffness(stiffness);
}

float WheelJoint::GetStiffness() const
{
    return this->joint->GetStiffness();
}

void WheelJoint::SetDamping(float damping)
{
    this->joint->SetDamping(damping);
}

float WheelJoint::GetDamping() const
{
    return this->joint->GetDamping();
}

int WheelJoint::GetAxis(lua_State* L)
{
    auto axis = this->joint->GetLocalAxisA();
    this->GetBodyA()->GetWorldVector(axis.x, axis.y, axis.x, axis.y);

    lua_pushnumber(L, axis.x);
    lua_pushnumber(L, axis.y);

    return 2;
}
