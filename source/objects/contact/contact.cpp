#include <objects/contact/contact.hpp>

#include <modules/physics/physics.hpp>

#include <objects/world/world.hpp>

using namespace love;

Type Contact::type("Contact", &Object::type);

Contact::Contact(World* world, b2Contact* contact) : contact(contact), world(world)
{
    world->RegisterObject(contact, this);
}

Contact::~Contact()
{
    this->Invalidate();
}

void Contact::Invalidate()
{
    if (this->contact != nullptr)
    {
        this->world->UnregisterObject(this->contact);
        this->contact = nullptr;
    }
}

bool Contact::IsValid()
{
    return this->contact != nullptr;
}

int Contact::GetPositions(lua_State* L)
{
    luax::AssertArgCount<1, 1>(L);

    b2WorldManifold manifold {};
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
    luax::AssertArgCount<1, 1>(L);

    b2WorldManifold manifold {};
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

bool Contact::IsEnabled()
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

void Contact::GetShapes(Shape*& shapeA, Shape*& shapeB)
{
    shapeA = (Shape*)this->contact->GetFixtureA()->GetUserData().pointer;
    shapeB = (Shape*)this->contact->GetFixtureB()->GetUserData().pointer;

    if (shapeA == nullptr || shapeB == nullptr)
        throw love::Exception("A Shape has escaped Memoizer!");
}
