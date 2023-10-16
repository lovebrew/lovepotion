#include <objects/joint/types/gearjoint/gearjoint.hpp>

#include <common/exception.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

using namespace love;

Type GearJoint::type("GearJoint", &Joint::type);

GearJoint::GearJoint(Joint* a, Joint* b, float ratio, bool collideConnected) :
    Joint(a->bodyA, b->bodyB),
    joint(nullptr)
{
    b2GearJointDef definition;
    definition.joint1           = a->joint;
    definition.joint2           = b->joint;
    definition.bodyA            = a->bodyB->body;
    definition.bodyB            = b->bodyB->body;
    definition.ratio            = ratio;
    definition.collideConnected = collideConnected;

    this->joint = (b2GearJoint*)this->CreateJoint(&definition);
}

GearJoint::~GearJoint()
{}

void GearJoint::SetRatio(float ratio)
{
    this->joint->SetRatio(ratio);
}

float GearJoint::GetRatio() const
{
    return this->joint->GetRatio();
}

Joint* GearJoint::GetJointA() const
{
    auto* b2joint = this->joint->GetJoint1();
    if (b2joint == nullptr)
        return nullptr;

    auto* joint = (Joint*)(b2joint->GetUserData().pointer);
    if (joint == nullptr)
        throw love::Exception("A joint has escaped Memoizer!");

    return joint;
}

Joint* GearJoint::GetJointB() const
{
    auto* b2joint = this->joint->GetJoint2();
    if (b2joint == nullptr)
        return nullptr;

    auto* joint = (Joint*)(b2joint->GetUserData().pointer);
    if (joint == nullptr)
        throw love::Exception("A joint has escaped Memoizer!");

    return joint;
}
