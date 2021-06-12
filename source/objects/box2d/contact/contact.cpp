#include "objects/box2d/contact/contact.h"

using namespace love;

love::Type Contact::type("Contact", &Object::type);

Contact::Contact(World* world, b2Contact* contact) : contact(contact), world(world)
{
    this->world->RegisterObject(contact, this);
}

Contact::~Contact()
{
    this->Invalidate();
}

void Contact::Invalidate()
{
    if (this->contact != NULL)
    {
        this->world->UnRegisterObject(contact);
        this->contact = NULL;
    }
}

bool Contact::IsValid()
{
    return this->contact != NULL;
}

int Contact::GetPositions(lua_State* L)
{
    Luax::AssertArgc<1, 1>(L);

    b2WorldManifold manifold;
    this->contact->GetWorldManifold(&manifold);

    int points = this->contact->GetManifold()->pointCount;

    for (int index = 0; index < points; index++)
    {
        b2Vec2 position = Physics::ScaleUp(manifold.points[index]);

        lua_pushnumber(L, position.x);
        lua_pushnumber(L, position.y);
    }

    return points * 2;
}

int Contact::GetNormal(lua_State* L)
{
    Luax::AssertArgc<1>(L);

    b2WorldManifold manifold;
    this->contact->GetWorldManifold(&manifold);

    lua_pushnumber(L, manifold.normal.x);
    lua_pushnumber(L, manifold.normal.y);

    return 2;
}

float Contact::GetFriction() const
{
    return this->contact->GetFriction();
}

float Contact::GetRestitution() const
{
    return this->contact->GetRestitution();
}

bool Contact::IsEnabled() const
{
    return this->contact->IsEnabled();
}

bool Contact::IsTouching() const
{
    return this->contact->IsTouching();
}

void Contact::SetFriction(float friction)
{
    this->contact->SetFriction(friction);
}

void Contact::SetRestitution(float restitution)
{
    this->contact->SetRestitution(restitution);
}

void Contact::SetEnabled(bool enabled)
{
    this->contact->SetEnabled(enabled);
}

void Contact::ResetFriction()
{
    this->contact->ResetFriction();
}

void Contact::ResetRestitution()
{
    this->contact->ResetRestitution();
}

void Contact::SetTangentSpeed(float speed)
{
    this->contact->SetTangentSpeed(speed);
}

float Contact::GetTangentSpeed() const
{
    return this->contact->GetTangentSpeed();
}

void Contact::GetChildren(int& childA, int& childB)
{
    childA = this->contact->GetChildIndexA();
    childB = this->contact->GetChildIndexB();
}

void Contact::GetFixtures(Fixture*& fixtureA, Fixture*& fixtureB)
{
    fixtureA = (Fixture*)this->world->FindObject(this->contact->GetFixtureA());
    fixtureB = (Fixture*)this->world->FindObject(this->contact->GetFixtureB());

    if (!fixtureA || !fixtureB)
        throw love::Exception("A fixture has escaped Memorizer!");
}
