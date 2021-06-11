#include "mousejoint/mousejoint.h"

#include "modules/physics/physics.h"
#include "world/world.h"

using namespace love;

love::Type MouseJoint::type("MouseJoint", &Joint::type);

MouseJoint::MouseJoint(Body* body, float x, float y) : Joint(body), joint(NULL)
{
    if (body->GetType() == Body::BODY_KINEMATIC)
        throw love::Exception("Cannot attach a MouseJoint to a kinematic body");

    b2MouseJointDef definition;
    definition.bodyA    = body->world->GetGroundBody();
    definition.bodyB    = body->body;
    definition.maxForce = 1000.0f * body->body->GetMass();
    definition.target   = Physics::ScaleDown(b2Vec2(x, y));

    this->joint = (b2MouseJoint*)this->CreateJoint(&definition);
}

MouseJoint::~MouseJoint()
{}

void MouseJoint::SetTarget(float x, float y)
{
    b2Vec2 target = Physics::ScaleDown(b2Vec2(x, y));
    this->joint->SetTarget(target);
}

int MouseJoint::GetTarget(lua_State* L)
{
    b2Vec2 target = this->joint->GetTarget();

    lua_pushnumber(L, Physics::ScaleUp(target.x));
    lua_pushnumber(L, Physics::ScaleUp(target.y));

    return 2;
}

void MouseJoint::SetMaxForce(float force)
{
    this->joint->SetMaxForce(Physics::ScaleDown(force));
}

float MouseJoint::GetMaxForce() const
{
    return Physics::ScaleUp(this->joint->GetMaxForce());
}

void MouseJoint::SetFrequency(float hz)
{
    /*
    ** This is kind of a crappy check. The Stiffness is used in an internal
    ** box2d calculation whose result must be > FLT_EPSILON, but other variables
    ** go into that calculation
    */
    if (hz <= FLT_EPSILON * 2)
        throw love::Exception("MouseJoint Stiffness must be a positive number.");

    float stiffness, damping;
    b2LinearStiffness(stiffness, damping, hz, this->GetDampingRatio(), this->joint->GetBodyA(),
                      this->joint->GetBodyB());

    this->joint->SetStiffness(stiffness);
}

float MouseJoint::GetFrequency() const
{
    float frequency, ratio;
    Physics::b2LinearFrequency(frequency, ratio, this->joint->GetStiffness(),
                               this->joint->GetDamping(), this->joint->GetBodyA(),
                               this->joint->GetBodyB());

    return frequency;
}

void MouseJoint::SetDampingRatio(float ratio)
{
    float stiffness, damping;
    b2LinearStiffness(stiffness, damping, this->GetFrequency(), ratio, this->joint->GetBodyA(),
                      this->joint->GetBodyB());

    this->joint->SetDamping(damping);
}

float MouseJoint::GetDampingRatio() const
{
    float frequency, ratio;
    Physics::b2LinearFrequency(frequency, ratio, this->joint->GetStiffness(),
                               this->joint->GetDamping(), this->joint->GetBodyA(),
                               this->joint->GetBodyB());

    return ratio;
}

void MouseJoint::SetStiffness(float stiffness)
{
    /*
     ** This is kind of a crappy check. The Stiffness is used in an internal
     ** box2d calculation whose result must be > FLT_EPSILON, but other variables
     ** go into that calculation
     */
    if (stiffness <= FLT_EPSILON * 2)
        throw love::Exception("MouseJoint Stiffness must be a positive number.");

    this->joint->SetStiffness(stiffness);
}

float MouseJoint::GetStiffness() const
{
    return this->joint->GetStiffness();
}

void MouseJoint::SetDamping(float damping)
{
    this->joint->SetDamping(damping);
}

float MouseJoint::GetDamping() const
{
    return this->joint->GetDamping();
}

Body* MouseJoint::GetBodyA() const
{
    return Joint::GetBodyB();
}

Body* MouseJoint::GetBodyB() const
{
    return nullptr;
}
