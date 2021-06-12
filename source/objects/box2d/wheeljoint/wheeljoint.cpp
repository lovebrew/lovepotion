#include "wheeljoint/wheeljoint.h"
#include "modules/physics/physics.h"

using namespace love;

love::Type WheelJoint::type("WheelJoint", &Joint::type);

WheelJoint::WheelJoint(Body* a, Body* b, float xA, float yA, float xB, float yB, float anchorX,
                       float anchorY, bool collideConnected) :
    Joint(a, b),
    joint(NULL)
{
    b2WheelJointDef definition;
    definition.Initialize(a->body, b->body, Physics::ScaleDown(b2Vec2(xA, yA)),
                          b2Vec2(anchorX, anchorY));

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
    this->joint->SetMaxMotorTorque(Physics::ScaleDown(Physics::ScaleDown(torque)));
}

float WheelJoint::GetMaxMotorTorque() const
{
    return Physics::ScaleUp(Physics::ScaleUp(this->GetMaxMotorTorque()));
}

float WheelJoint::GetMotorTorque(float invDt) const
{
    return Physics::ScaleUp(Physics::ScaleUp(this->joint->GetMotorTorque(invDt)));
}

void WheelJoint::SetFrequency(float hz)
{
    float stiffness, damping;
    b2AngularStiffness(stiffness, damping, hz, this->GetDampingRatio(), this->joint->GetBodyA(),
                       this->joint->GetBodyB());

    this->joint->SetStiffness(stiffness);
}

float WheelJoint::GetFrequency() const
{
    float frequency, ratio;
    Physics::b2AngularFrequency(frequency, ratio, this->joint->GetStiffness(),
                                this->joint->GetDamping(), this->joint->GetBodyA(),
                                this->joint->GetBodyB());

    return frequency;
}

void WheelJoint::SetDampingRatio(float ratio)
{
    float stiffness, damping;
    b2AngularStiffness(stiffness, damping, this->GetFrequency(), ratio, this->joint->GetBodyA(),
                       this->joint->GetBodyB());

    this->joint->SetDamping(ratio);
}

float WheelJoint::GetDampingRatio() const
{
    float frequency, ratio;
    Physics::b2AngularFrequency(frequency, ratio, this->joint->GetStiffness(),
                                this->joint->GetDamping(), this->joint->GetBodyA(),
                                this->joint->GetBodyB());

    return ratio;
}

void WheelJoint::SetStiffness(float k)
{
    this->joint->SetStiffness(k);
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
    b2Vec2 axis = this->joint->GetLocalAxisA();

    this->GetBodyA()->GetWorldVector(axis.x, axis.y, axis.x, axis.y);

    lua_pushnumber(L, axis.x);
    lua_pushnumber(L, axis.y);

    return 2;
}
