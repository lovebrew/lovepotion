#include "objects/box2d/body/body.h"

#include "modules/physics/physics.h"

#include "contact/contact.h"
#include "fixture/fixture.h"
#include "joint/joint.h"
#include "shape/shape.h"
#include "world/world.h"

#include "joint/wrap_joint.h"

using namespace love;

love::Type Body::type("Body", &Object::type);

Body::Body(World* world, b2Vec2 position, Body::Type type) : world(world), userdata(nullptr)
{
    this->userdata = new BodyUserdata { .ref = nullptr };

    b2BodyDef bodyDefinition;
    bodyDefinition.position         = Physics::ScaleDown(position);
    bodyDefinition.userData.pointer = (uintptr_t)this->userdata;

    this->body = this->world->world->CreateBody(&bodyDefinition);

    this->Retain();
    this->SetType(type);

    this->world->RegisterObject(body, this);
}

Body::~Body()
{
    if (!this->userdata)
        return;

    if (this->userdata->ref)
        delete this->userdata->ref;

    delete this->userdata;
}

float Body::GetX()
{
    return Physics::ScaleUp(this->body->GetPosition().x);
}

float Body::GetY()
{
    return Physics::ScaleUp(this->body->GetPosition().y);
}

void Body::GetPosition(float& xPosition, float& yPosition)
{
    b2Vec2 v = Physics::ScaleUp(this->body->GetPosition());

    xPosition = v.x;
    yPosition = v.y;
}

void Body::GetLinearVelocity(float& xVelocity, float& yVelocity)
{
    b2Vec2 v = Physics::ScaleUp(this->body->GetLinearVelocity());

    xVelocity = v.x;
    yVelocity = v.y;
}

float Body::GetAngle()
{
    return this->body->GetAngle();
}

void Body::GetWorldCenter(float& xWorld, float& yWorld)
{
    b2Vec2 v = Physics::ScaleUp(this->body->GetWorldCenter());

    xWorld = v.x;
    yWorld = v.y;
}

void Body::GetLocalCenter(float& xLocal, float& yLocal)
{
    b2Vec2 v = Physics::ScaleUp(this->body->GetLocalCenter());

    xLocal = v.x;
    yLocal = v.y;
}

float Body::GetAngularVelocity() const
{
    return this->body->GetAngularVelocity();
}

void Body::GetKinematicState(b2Vec2& outPosition, float& outAngle, b2Vec2& outVelocity,
                             float& outAngularVelocity) const
{
    outPosition        = Physics::ScaleUp(this->body->GetPosition());
    outAngle           = body->GetAngle();
    outVelocity        = Physics::ScaleUp(this->body->GetLinearVelocity());
    outAngularVelocity = body->GetAngularVelocity();
}

float Body::GetMass() const
{
    return this->body->GetMass();
}

float Body::GetInertia() const
{
    return Physics::ScaleUp(Physics::ScaleUp(this->body->GetInertia()));
}

int Body::GetMassData(lua_State* L)
{
    b2MassData data;
    this->body->GetMassData(&data);

    b2Vec2 center = Physics::ScaleUp(data.center);

    lua_pushnumber(L, center.x);
    lua_pushnumber(L, center.y);
    lua_pushnumber(L, data.mass);
    lua_pushnumber(L, Physics::ScaleUp(Physics::ScaleUp(data.I)));

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
        case b2_staticBody:
            return BODY_STATIC;
        case b2_dynamicBody:
            return BODY_DYNAMIC;
        case b2_kinematicBody:
            return BODY_KINEMATIC;
        default:
            return BODY_INVALID;
    }
}

void Body::ApplyLinearImpulse(float jx, float jy, bool wake)
{
    this->body->ApplyLinearImpulse(Physics::ScaleDown(b2Vec2(jx, jy)), this->body->GetWorldCenter(),
                                   wake);
}

void Body::ApplyLinearImpulse(float jx, float jy, float rx, float ry, bool wake)
{
    this->body->ApplyLinearImpulse(Physics::ScaleDown(b2Vec2(jx, jy)),
                                   Physics::ScaleDown(b2Vec2(rx, ry)), wake);
}

void Body::ApplyAngularImpulse(float impulse, bool wake)
{
    // Angular impulse is in kg*m^2/s, meaning it needs to be scaled twice
    this->body->ApplyAngularImpulse(Physics::ScaleDown(Physics::ScaleDown(impulse)), wake);
}

void Body::ApplyTorque(float t, bool wake)
{
    // Torque is in N*m, or kg*m^2/s^2, meaning it also needs to be scaled twice
    this->body->ApplyTorque(Physics::ScaleDown(Physics::ScaleDown(t)), wake);
}

void Body::ApplyForce(float fx, float fy, float rx, float ry, bool wake)
{
    this->body->ApplyForce(Physics::ScaleDown(b2Vec2(fx, fy)), Physics::ScaleDown(b2Vec2(rx, ry)),
                           wake);
}

void Body::ApplyForce(float fx, float fy, bool wake)
{
    this->body->ApplyForceToCenter(Physics::ScaleDown(b2Vec2(fx, fy)), wake);
}

void Body::SetX(float x)
{
    body->SetTransform(Physics::ScaleDown(b2Vec2(x, this->GetY())), this->GetAngle());
}

void Body::SetY(float y)
{
    this->body->SetTransform(Physics::ScaleDown(b2Vec2(this->GetX(), y)), this->GetAngle());
}

void Body::SetLinearVelocity(float x, float y)
{
    this->body->SetLinearVelocity(Physics::ScaleDown(b2Vec2(x, y)));
}

void Body::SetAngle(float d)
{
    this->body->SetTransform(this->body->GetPosition(), d);
}

void Body::SetAngularVelocity(float r)
{
    this->body->SetAngularVelocity(r);
}

void Body::SetKinematicState(b2Vec2 pos, float a, b2Vec2 vel, float da)
{
    this->body->SetTransform(Physics::ScaleDown(pos), a);
    this->body->SetLinearVelocity(Physics::ScaleDown(vel));
    this->body->SetAngularVelocity(da);
}

void Body::SetPosition(float x, float y)
{
    this->body->SetTransform(Physics::ScaleDown(b2Vec2(x, y)), this->body->GetAngle());
}

void Body::SetAngularDamping(float d)
{
    this->body->SetAngularDamping(d);
}

void Body::SetLinearDamping(float d)
{
    this->body->SetLinearDamping(d);
}

void Body::ResetMassData()
{
    this->body->ResetMassData();
}

void Body::SetMassData(float x, float y, float m, float i)
{
    b2MassData massData;

    massData.center = Physics::ScaleDown(b2Vec2(x, y));
    massData.mass   = m;
    massData.I      = Physics::ScaleDown(Physics::ScaleDown(i));

    this->body->SetMassData(&massData);
}

void Body::SetMass(float m)
{
    b2MassData data;
    this->body->GetMassData(&data);

    data.mass = m;
    this->body->SetMassData(&data);
}

void Body::SetInertia(float i)
{
    b2MassData massData;
    massData.center = this->body->GetLocalCenter();
    massData.mass   = this->body->GetMass();
    massData.I      = Physics::ScaleDown(Physics::ScaleDown(i));

    this->body->SetMassData(&massData);
}

void Body::SetGravityScale(float scale)
{
    this->body->SetGravityScale(scale);
}

void Body::SetActive(bool active)
{
    this->body->SetEnabled(active);
}

void Body::SetType(Body::Type type)
{
    switch (type)
    {
        case Body::BODY_STATIC:
            body->SetType(b2_staticBody);
            break;
        case Body::BODY_DYNAMIC:
            body->SetType(b2_dynamicBody);
            break;
        case Body::BODY_KINEMATIC:
            body->SetType(b2_kinematicBody);
            break;
        default:
            break;
    }
}

void Body::GetWorldPoint(float x, float y, float& xOut, float& yOut)
{
    b2Vec2 v = Physics::ScaleUp(this->body->GetWorldPoint(Physics::ScaleDown(b2Vec2(x, y))));

    xOut = v.x;
    yOut = v.y;
}

void Body::GetWorldVector(float x, float y, float& xOut, float& yOut)
{
    b2Vec2 v = Physics::ScaleUp(this->body->GetWorldVector(Physics::ScaleDown(b2Vec2(x, y))));

    xOut = v.x;
    yOut = v.y;
}

int Body::GetWorldPoints(lua_State* L)
{
    int argc   = lua_gettop(L);
    int vcount = argc / 2;

    // at least one point
    Luax::AssertArgc<2>(L);

    for (int i = 0; i < vcount; i++)
    {
        float x = lua_tonumber(L, 1);
        float y = lua_tonumber(L, 2);

        // Remove them, so we don't run out of stack space
        lua_remove(L, 1);
        lua_remove(L, 1);

        // Time for scaling
        b2Vec2 bodyPoint  = this->body->GetWorldPoint(Physics::ScaleDown(b2Vec2(x, y)));
        b2Vec2 worldPoint = Physics::ScaleUp(bodyPoint);

        // And then we push the result
        lua_pushnumber(L, worldPoint.x);
        lua_pushnumber(L, worldPoint.y);
    }

    return argc;
}

void Body::GetLocalPoint(float x, float y, float& xOut, float& yOut)
{
    b2Vec2 v = Physics::ScaleUp(body->GetLocalPoint(Physics::ScaleDown(b2Vec2(x, y))));

    xOut = v.x;
    yOut = v.y;
}

void Body::GetLocalVector(float x, float y, float& xOut, float& yOut)
{
    b2Vec2 v = Physics::ScaleUp(body->GetLocalVector(Physics::ScaleDown(b2Vec2(x, y))));

    xOut = v.x;
    yOut = v.y;
}

int Body::GetLocalPoints(lua_State* L)
{
    int argc   = lua_gettop(L);
    int vcount = (int)argc / 2;

    // at least one point
    Luax::AssertArgc<2>(L);

    for (int i = 0; i < vcount; i++)
    {
        float x = (float)lua_tonumber(L, 1);
        float y = (float)lua_tonumber(L, 2);

        // Remove them, so we don't run out of stack space
        lua_remove(L, 1);
        lua_remove(L, 1);

        // Time for scaling
        b2Vec2 point =
            Physics::ScaleUp(this->body->GetLocalPoint(Physics::ScaleDown(b2Vec2(x, y))));

        // And then we push the result
        lua_pushnumber(L, point.x);
        lua_pushnumber(L, point.y);
    }

    return argc;
}

void Body::GetLinearVelocityFromWorldPoint(float x, float y, float& xOut, float& yOut)
{
    b2Vec2 v = Physics::ScaleUp(
        this->body->GetLinearVelocityFromWorldPoint(Physics::ScaleDown(b2Vec2(x, y))));

    xOut = v.x;
    yOut = v.y;
}

void Body::GetLinearVelocityFromLocalPoint(float x, float y, float& xOut, float& yOut)
{
    b2Vec2 v = Physics::ScaleUp(
        this->body->GetLinearVelocityFromLocalPoint(Physics::ScaleDown(b2Vec2(x, y))));

    xOut = v.x;
    yOut = v.y;
}

bool Body::IsBullet() const
{
    return this->body->IsBullet();
}

void Body::SetBullet(bool bullet)
{
    return this->body->SetBullet(bullet);
}

bool Body::IsEnabled() const
{
    return this->body->IsEnabled();
}

bool Body::IsAwake() const
{
    return this->body->IsAwake();
}

void Body::SetSleepingAllowed(bool allow)
{
    this->body->SetSleepingAllowed(allow);
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
    const b2ContactEdge* contactEdge = body->GetContactList();

    b2Body* otherbody = other->body;

    while (contactEdge != nullptr)
    {
        if (contactEdge->other == otherbody && contactEdge->contact != nullptr &&
            contactEdge->contact->IsTouching())
            return true;

        contactEdge = contactEdge->next;
    }

    return false;
}

World* Body::GetWorld() const
{
    return this->world;
}

int Body::GetFixtures(lua_State* L) const
{
    lua_newtable(L);
    b2Fixture* fixtureList = body->GetFixtureList();

    int i = 1;

    do
    {
        if (!fixtureList)
            break;

        Fixture* fixture = (Fixture*)this->world->FindObject(fixtureList);

        if (!fixture)
            throw love::Exception("A fixture has escaped Memoizer!");

        Luax::PushType(L, fixture);
        lua_rawseti(L, -2, i);

        i++;
    } while ((fixtureList = fixtureList->GetNext()));

    return 1;
}

int Body::GetJoints(lua_State* L) const
{
    lua_newtable(L);
    const b2JointEdge* jointEdge = body->GetJointList();

    int i = 1;

    do
    {
        if (!jointEdge)
            break;

        Joint* joint = (Joint*)this->world->FindObject(jointEdge->joint);

        if (!joint)
            throw love::Exception("A joint has escaped Memoizer!");

        Wrap_Joint::PushJoint(L, joint);
        lua_rawseti(L, -2, i);

        i++;
    } while ((jointEdge = jointEdge->next));

    return 1;
}

int Body::GetContacts(lua_State* L) const
{
    lua_newtable(L);
    const b2ContactEdge* contactEdge = body->GetContactList();

    int i = 1;

    do
    {
        if (!contactEdge)
            break;

        Contact* contact = (Contact*)this->world->FindObject(contactEdge->contact);

        if (!contact)
            contact = new Contact(world, contactEdge->contact);
        else
            contact->Retain();

        Luax::PushType(L, contact);
        contact->Release();
        lua_rawseti(L, -2, i);

        i++;
    } while ((contactEdge = contactEdge->next));

    return 1;
}

void Body::Destroy()
{
    if (this->world->world->IsLocked())
    {
        // Called during time step. Save reference for destruction afterwards.
        this->Retain();
        world->destructBodies.push_back(this);

        return;
    }

    this->world->world->DestroyBody(body);
    this->world->UnRegisterObject(body);

    body = NULL;

    // Remove userdata reference to avoid it sticking around after GC
    if (this->userdata && this->userdata->ref)
        this->userdata->ref->UnRef();

    // Box2D body destroyed. Release its reference to the love Body.
    this->Release();
}

int Body::SetUserData(lua_State* L)
{
    Luax::AssertArgc<1, 1>(L);

    if (this->userdata == nullptr)
    {
        this->userdata              = new BodyUserdata();
        body->GetUserData().pointer = (uintptr_t)this->userdata;
    }

    if (!this->userdata->ref)
        this->userdata->ref = new Reference();

    this->userdata->ref->Ref(L);

    return 0;
}

int Body::GetUserData(lua_State* L)
{
    if (this->userdata != nullptr && this->userdata->ref != nullptr)
        this->userdata->ref->Push(L);
    else
        lua_pushnil(L);

    return 1;
}

bool Body::GetConstant(const char* in, Body::Type& out)
{
    return types.Find(in, out);
}

bool Body::GetConstant(Body::Type in, const char*& out)
{
    return types.Find(in, out);
}

std::vector<const char*> Body::GetConstants(Body::Type)
{
    return types.GetNames();
}

// clang-format off

constexpr StringMap<Body::Type, Body::BODY_MAX_ENUM>::Entry typeEntries[] =
{
    { "static",    Body::BODY_STATIC    },
    { "dynamic",   Body::BODY_DYNAMIC   },
    { "kinematic", Body::BODY_KINEMATIC }
};

constinit const StringMap<Body::Type, Body::BODY_MAX_ENUM> Body::types(typeEntries);

// clang-format on
