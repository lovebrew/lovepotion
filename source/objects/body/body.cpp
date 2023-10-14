#include <objects/body/body.hpp>

#include <objects/contact/contact.hpp>

#include <objects/joint/joint.hpp>
#include <objects/joint/wrap_joint.hpp>

#include <objects/shape/shape.hpp>
#include <objects/shape/wrap_shape.hpp>

#include <objects/world/world.hpp>

#include <modules/physics/physics.hpp>

using namespace love;

Type love::Body::type("Body", &Object::type);

Body::Body(World* world, b2Vec2 position, Body::Type type) : world(world), hasCustomMass(false)
{
    b2BodyDef bodyDef {};
    bodyDef.position         = Physics::ScaleDown(position);
    bodyDef.userData.pointer = (uintptr_t)this;

    this->body = world->world->CreateBody(&bodyDef);
    this->Retain();
    this->SetType(type);
}

Body::~Body()
{
    if (this->reference)
        delete this->reference;
}

float Body::GetX()
{
    return Physics::ScaleUp(this->body->GetPosition().x);
}

float Body::GetY()
{
    return Physics::ScaleUp(this->body->GetPosition().y);
}

void Body::GetPosition(float& x, float& y)
{
    const auto& position = Physics::ScaleUp(this->body->GetPosition());

    x = position.x;
    y = position.y;
}

void Body::GetLinearVelocity(float& x, float& y)
{
    const auto& velocity = Physics::ScaleUp(this->body->GetLinearVelocity());

    x = velocity.x;
    y = velocity.y;
}

float Body::GetAngle()
{
    return this->body->GetAngle();
}

void Body::GetWorldCenter(float& x, float& y)
{
    const auto& center = Physics::ScaleUp(this->body->GetWorldCenter());

    x = center.x;
    y = center.y;
}

void Body::GetLocalCenter(float& x, float& y)
{
    const auto& center = Physics::ScaleUp(this->body->GetLocalCenter());

    x = center.x;
    y = center.y;
}

float Body::GetAngularVelocity()
{
    return this->body->GetAngularVelocity();
}

void Body::GetKinematicState(b2Vec2& position, float& angle, b2Vec2& velocity,
                             float& angularVelocity) const
{
    position        = this->body->GetPosition();
    angle           = this->body->GetAngle();
    velocity        = this->body->GetLinearVelocity();
    angularVelocity = this->body->GetAngularVelocity();
}

float Body::GetMass() const
{
    return this->body->GetMass();
}

float Body::GetInertia() const
{
    return Physics::ScaleUp(this->body->GetInertia());
}

int Body::GetMassData(lua_State* L)
{
    b2MassData massData {};
    this->body->GetMassData(&massData);

    const auto& center = Physics::ScaleUp(massData.center);

    lua_pushnumber(L, center.x);
    lua_pushnumber(L, center.y);
    lua_pushnumber(L, massData.mass);
    lua_pushnumber(L, Physics::ScaleUp(massData.I));

    return 4;
}

float Body::GetAngularDamping() const
{
    return this->body->GetAngularDamping();
}

float Body::GetLinearDamping() const
{
    return this->body->GetLinearDamping();
}

float Body::GetGravityScale() const
{
    return this->body->GetGravityScale();
}

Body::Type Body::GetType() const
{
    switch (this->body->GetType())
    {
        case b2_kinematicBody:
            return Body::BODY_KINEMATIC;
        case b2_dynamicBody:
            return Body::BODY_DYNAMIC;
        case b2_staticBody:
        default:
            return Body::BODY_STATIC;
    }
}

void Body::SetType(Body::Type type)
{
    switch (type)
    {
        case Body::BODY_STATIC:
        {
            this->body->SetType(b2_staticBody);
            break;
        }
        case Body::BODY_KINEMATIC:
        {
            this->body->SetType(b2_kinematicBody);
            break;
        }
        case Body::BODY_DYNAMIC:
        {
            this->body->SetType(b2_dynamicBody);
            break;
        }
        default:
            break;
    }
}

void Body::ApplyLinearImpulse(float x, float y, bool wake)
{
    const auto impulse = Physics::ScaleDown(b2Vec2(x, y));

    this->body->ApplyLinearImpulse(impulse, this->body->GetWorldCenter(), wake);
}

void Body::ApplyLinearImpulse(float x, float y, float offsetX, float offsetY, bool wake)
{
    const auto impulse = Physics::ScaleDown(b2Vec2(x, y));
    const auto offset  = Physics::ScaleDown(b2Vec2(offsetX, offsetY));

    this->body->ApplyLinearImpulse(impulse, offset, wake);
}

void Body::ApplyAngularImpulse(float impulse, bool wake)
{
    const auto force = Physics::ScaleDown(Physics::ScaleDown(impulse));

    this->body->ApplyAngularImpulse(force, wake);
}

void Body::ApplyTorque(float torque, bool wake)
{
    const auto force = Physics::ScaleDown(Physics::ScaleDown(torque));

    this->body->ApplyTorque(force, wake);
}

void Body::ApplyForce(float x, float y, bool wake)
{
    const auto force = Physics::ScaleDown(b2Vec2(x, y));

    this->body->ApplyForceToCenter(force, wake);
}

void Body::ApplyForce(float x, float y, float offsetX, float offsetY, bool wake)
{
    const auto force  = Physics::ScaleDown(b2Vec2(x, y));
    const auto offset = Physics::ScaleDown(b2Vec2(offsetX, offsetY));

    this->body->ApplyForce(force, offset, wake);
}

void Body::SetX(float x)
{
    const auto y        = this->GetY();
    const auto position = Physics::ScaleDown(b2Vec2(x, y));

    this->body->SetTransform(position, this->GetAngle());
}

void Body::SetY(float y)
{
    const auto x        = this->GetX();
    const auto position = Physics::ScaleDown(b2Vec2(x, y));

    this->body->SetTransform(position, this->GetAngle());
}

void Body::SetLinearVelocity(float x, float y)
{
    const auto velocity = Physics::ScaleDown(b2Vec2(x, y));

    this->body->SetLinearVelocity(velocity);
}

void Body::SetAngle(float angle)
{
    this->body->SetTransform(this->body->GetPosition(), angle);
}

void Body::SetAngularVelocity(float rotation)
{
    this->body->SetAngularVelocity(rotation);
}

void Body::SetKinematicState(b2Vec2 position, float angle, b2Vec2 velocity, float angularVelocity)
{
    this->body->SetTransform(Physics::ScaleDown(position), angle);
    this->body->SetLinearVelocity(Physics::ScaleDown(velocity));
    this->body->SetAngularVelocity(angularVelocity);
}

void Body::SetPosition(float x, float y)
{
    const auto position = Physics::ScaleDown(b2Vec2(x, y));

    this->body->SetTransform(position, this->GetAngle());
}

void Body::SetAngularDamping(float damping)
{
    this->body->SetAngularDamping(damping);
}

void Body::SetLinearDamping(float damping)
{
    this->body->SetLinearDamping(damping);
}

void Body::ResetMassData()
{
    this->body->ResetMassData();
    this->hasCustomMass = false;
}

void Body::SetMassData(float x, float y, float mass, float inertia)
{
    b2MassData massData {};
    massData.center = Physics::ScaleDown(b2Vec2(x, y));
    massData.mass   = mass;
    massData.I      = Physics::ScaleDown(Physics::ScaleDown(inertia));

    this->body->SetMassData(&massData);
    this->hasCustomMass = true;
}

void Body::SetMass(float mass)
{
    b2MassData massData {};
    this->body->GetMassData(&massData);
    massData.mass = mass;

    this->body->SetMassData(&massData);
    this->hasCustomMass = true;
}

void Body::SetInertia(float inertia)
{
    b2MassData massData {};
    massData.center = this->body->GetLocalCenter();
    massData.mass   = this->body->GetMass();
    massData.I      = Physics::ScaleDown(Physics::ScaleDown(inertia));

    this->body->SetMassData(&massData);
    this->hasCustomMass = true;
}

void Body::SetGravityScale(float scale)
{
    this->body->SetGravityScale(scale);
}

void Body::GetWorldPoint(float localX, float localY, float& worldX, float& worldY) const
{
    const auto worldPoint = this->body->GetWorldPoint(Physics::ScaleDown(b2Vec2(localX, localY)));
    const auto point      = Physics::ScaleUp(worldPoint);

    worldX = point.x;
    worldY = point.y;
}

void Body::GetWorldVector(float localX, float localY, float& worldX, float& worldY) const
{
    const auto worldVector = this->body->GetWorldVector(Physics::ScaleDown(b2Vec2(localX, localY)));
    const auto vector      = Physics::ScaleUp(worldVector);

    worldX = vector.x;
    worldY = vector.y;
}

int Body::GetWorldPoints(lua_State* L)
{
    const auto count = lua_gettop(L);
    const auto size  = count / 2;

    luax::AssertArgCount<2>(L);

    for (int index = 0; index < size; index++)
    {
        float x = lua_tonumber(L, 1);
        float y = lua_tonumber(L, 2);

        lua_remove(L, 1);
        lua_remove(L, 1);

        const auto worldPoint = this->body->GetWorldPoint(Physics::ScaleDown(b2Vec2(x, y)));
        const auto point      = Physics::ScaleUp(worldPoint);

        lua_pushnumber(L, point.x);
        lua_pushnumber(L, point.y);
    }

    return count;
}

void Body::GetLocalPoint(float worldX, float worldY, float& localX, float& localY) const
{
    const auto localPoint = this->body->GetLocalPoint(Physics::ScaleDown(b2Vec2(worldX, worldY)));
    const auto point      = Physics::ScaleUp(localPoint);

    localX = point.x;
    localY = point.y;
}

void Body::GetLocalVector(float worldX, float worldY, float& localX, float& localY) const
{
    const auto localVector = this->body->GetLocalVector(Physics::ScaleDown(b2Vec2(worldX, worldY)));
    const auto vector      = Physics::ScaleUp(localVector);

    localX = vector.x;
    localY = vector.y;
}

int Body::GetLocalPoints(lua_State* L)
{
    const auto count = lua_gettop(L);
    const auto size  = count / 2;

    luax::AssertArgCount<2>(L);

    for (int index = 0; index < size; index++)
    {
        float x = lua_tonumber(L, 1);
        float y = lua_tonumber(L, 2);

        lua_remove(L, 1);
        lua_remove(L, 1);

        const auto localPoint = this->body->GetLocalPoint(Physics::ScaleDown(b2Vec2(x, y)));
        const auto point      = Physics::ScaleUp(localPoint);

        lua_pushnumber(L, point.x);
        lua_pushnumber(L, point.y);
    }

    return count;
}

void Body::GetLinearVelocityFromWorldPoint(float worldX, float worldY, float& localX,
                                           float& localY) const
{
    const auto linearVelocity =
        this->body->GetLinearVelocityFromWorldPoint(Physics::ScaleDown(b2Vec2(worldX, worldY)));
    const auto vector = Physics::ScaleUp(linearVelocity);

    localX = vector.x;
    localY = vector.y;
}

void Body::GetLinearVelocityFromLocalPoint(float localX, float localY, float& worldX,
                                           float& worldY) const
{
    const auto linearVelocity =
        this->body->GetLinearVelocityFromLocalPoint(Physics::ScaleDown(b2Vec2(localX, localY)));
    const auto vector = Physics::ScaleUp(linearVelocity);

    worldX = vector.x;
    worldY = vector.y;
}

bool Body::IsBullet() const
{
    return this->body->IsBullet();
}

void Body::SetBullet(bool bullet)
{
    this->body->SetBullet(bullet);
}

bool Body::IsEnabled() const
{
    return this->body->IsEnabled();
}

bool Body::IsAwake() const
{
    return this->body->IsAwake();
}

void Body::SetSleepingAllowed(bool allowed)
{
    this->body->SetSleepingAllowed(allowed);
}

bool Body::IsSleepingAllowed() const
{
    return this->body->IsSleepingAllowed();
}

void Body::SetEnabled(bool enabled)
{
    this->body->SetEnabled(enabled);
}

void Body::SetAwake(bool awake)
{
    this->body->SetAwake(awake);
}

void Body::SetFixedRotation(bool fixed)
{
    this->body->SetFixedRotation(fixed);
}

bool Body::IsFixedRotation() const
{
    return this->body->IsFixedRotation();
}

bool Body::IsTouching(Body* other) const
{
    const auto* contactEdge = body->GetContactList();
    auto* otherBody         = other->body;

    while (contactEdge != nullptr)
    {
        if (contactEdge->other == otherBody && contactEdge->contact != nullptr &&
            contactEdge->contact->IsTouching())
        {
            return true;
        }

        contactEdge = contactEdge->next;
    }

    return false;
}

World* Body::GetWorld() const
{
    return this->world;
}

Shape* Body::GetShape() const
{
    auto* fixtures = this->body->GetFixtureList();

    if (fixtures == nullptr)
        return nullptr;

    auto* shape = (Shape*)(fixtures->GetUserData().pointer);

    if (shape == nullptr)
        throw love::Exception("A Shape has escaped Memoizer!");

    return shape;
}

int Body::GetShapes(lua_State* L) const
{
    lua_newtable(L);
    auto* fixtures = this->body->GetFixtureList();

    int index = 1;

    do
    {
        if (fixtures == nullptr)
            break;

        auto* shape = (Shape*)(fixtures->GetUserData().pointer);

        if (shape == nullptr)
            throw love::Exception("A Joint has escaped Memoizer!");

        Wrap_Shape::PushShape(L, shape);
        lua_rawseti(L, -2, index);
        index++;
    } while ((fixtures = fixtures->GetNext()));

    return 1;
}

int Body::GetJoints(lua_State* L) const
{
    lua_newtable(L);
    const auto* joints = this->body->GetJointList();

    int index = 1;

    do
    {
        if (joints == nullptr)
            break;

        auto* joint = (Joint*)(joints->joint->GetUserData().pointer);

        if (joint == nullptr)
            throw love::Exception("A Joint has escaped Memoizer!");

        Wrap_Joint::PushJoint(L, joint);
        lua_rawseti(L, -2, index);
        index++;
    } while ((joints = joints->next));

    return 1;
}

int Body::GetContacts(lua_State* L) const
{
    lua_newtable(L);
    const auto* contacts = this->body->GetContactList();

    int index = 1;

    do
    {
        if (contacts == nullptr)
            break;

        auto* contact = (Contact*)this->world->FindObject(contacts->contact);

        if (contact == nullptr)
            contact = new Contact(this->world, contacts->contact);
        else
            contact->Retain();

        luax::PushType(L, contact);
        contact->Release();
        lua_rawseti(L, -2, index);
        index++;
    } while ((contacts = contacts->next));

    return 1;
}

void Body::Destroy()
{
    if (this->world->world->IsLocked())
    {
        this->Retain();
        this->world->destructBodies.push_back(this);
        return;
    }

    this->world->world->DestroyBody(this->body);
    this->body = nullptr;

    if (this->reference)
        this->reference->UnReference();

    this->Release();
}

int Body::SetUserdata(lua_State* L)
{
    luax::AssertArgCount<1>(L);

    if (!this->reference)
        this->reference = new Reference();

    this->reference->Create(L);
    return 0;
}

int Body::GetUserdata(lua_State* L)
{
    if (this->reference)
        this->reference->Push(L);
    else
        lua_pushnil(L);

    return 1;
}
