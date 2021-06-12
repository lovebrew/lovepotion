#include "gearjoint/gearjoint.h"

#include "modules/physics/physics.h"
#include "world/world.h"

using namespace love;

love::Type GearJoint::type("GearJoint", &Joint::type);

GearJoint::GearJoint(Joint* a, Joint* b, float ratio, bool collideConnected) :
    Joint(a->bodyB, b->bodyB),
    joint(NULL)
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
    b2Joint* bJoint = this->joint->GetJoint1();
    if (bJoint == nullptr)
        return nullptr;

    Joint* j = (Joint*)this->world->FindObject(bJoint);
    if (j == nullptr)
        throw love::Exception("A joint has escaped Memorizer!");

    return j;
}

Joint* GearJoint::GetJointB() const
{
    b2Joint* bJoint = this->joint->GetJoint2();
    if (bJoint == nullptr)
        return nullptr;

    Joint* j = (Joint*)this->world->FindObject(bJoint);
    if (j == nullptr)
        throw love::Exception("A joint has escaped Memorizer!");

    return j;
}
