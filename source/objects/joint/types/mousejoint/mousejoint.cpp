#include <objects/joint/types/mousejoint/mousejoint.hpp>

#include <modules/physics/physics.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

using namespace love;

Type MouseJoint::type("MouseJoint", &Joint::type);

MouseJoint::MouseJoint(Body* body, float x, float y) : Joint(body), joint(nullptr)
{
    if (body->GetType() == Body::BODY_KINEMATIC)
        throw love::Exception("Cannot attach a MouseJoint to a kinematic body.");

    b2MouseJointDef definition {};
    definition.bodyA    = body->world->GetGroundBody();
    definition.bodyB    = body->body;
    definition.maxForce = 1000.0f * body->body->GetMass();
    definition.target   = Physics::ScaleDown(b2Vec2(x, y));

    Physics::ComputeLinearStiffness(definition.stiffness, definition.damping, 5.0, 0.7,
                                    definition.bodyA, definition.bodyB);
    this->joint = (b2MouseJoint*)this->CreateJoint(&definition);
}

MouseJoint::~MouseJoint()
{}

void MouseJoint::SetTarget(float x, float y)
{
    this->joint->SetTarget(Physics::ScaleDown(b2Vec2(x, y)));
}

int MouseJoint::GetTarget(lua_State* L)
{
    const auto x = Physics::ScaleUp(this->joint->GetTarget().x);
    const auto y = Physics::ScaleUp(this->joint->GetTarget().y);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

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

void MouseJoint::SetStiffness(float stiffness)
{
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
