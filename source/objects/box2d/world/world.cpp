#include "objects/box2d/world/world.h"

#include "contact/contact.h"
#include "fixture/fixture.h"
#include "joint/joint.h"
#include "joint/wrap_joint.h"
#include "shape/shape.h"

using namespace love;

love::Type World::type("World", &Object::type);

void World::SayGoodbye(b2Fixture* bFixture)
{
    Fixture* fixture = (Fixture*)this->FindObject(bFixture);

    if (fixture)
        fixture->Destroy(true);
}

void World::SayGoodbye(b2Joint* bJoint)
{
    Joint* joint = (Joint*)this->FindObject(bJoint);

    if (joint)
        joint->DestroyJoint(true);
}

World::World() :
    world(nullptr),
    destructWorld(false),
    begin(this),
    end(this),
    presolve(this),
    postsolve(this)
{
    this->world = new b2World(b2Vec2(0, 0));

    this->world->SetAllowSleeping(true);
    this->world->SetContactListener(this);
    this->world->SetContactFilter(this);
    this->world->SetDestructionListener(this);

    b2BodyDef def;
    this->groundBody = this->world->CreateBody(&def);

    this->RegisterObject(this->world, this);
}

World::World(b2Vec2 gravity, bool sleep) :
    world(nullptr),
    destructWorld(false),
    begin(this),
    end(this),
    presolve(this),
    postsolve(this)
{
    this->world = new b2World(Physics::ScaleDown(gravity));

    this->world->SetAllowSleeping(sleep);
    this->world->SetContactListener(this);
    this->world->SetContactFilter(this);
    this->world->SetDestructionListener(this);

    b2BodyDef def;
    this->groundBody = world->CreateBody(&def);

    this->RegisterObject(world, this);
}

World::~World()
{
    this->Destroy();
}

/* box2d recommended defaults */
void World::Update(float dt)
{
    this->Update(dt, 8, 3);
}

void World::Update(float dt, int velocityIterations, int positionIterations)
{
    this->world->Step(dt, velocityIterations, positionIterations);

    // Destroy all objects marked during the time step.
    for (Body* b : this->destructBodies)
    {
        if (b->body != nullptr)
            b->Destroy();

        // Release for reference in vector.
        b->Release();
    }

    for (Fixture* f : this->destructFixtures)
    {
        if (f->IsValid())
            f->Destroy();

        // Release for reference in vector.
        f->Release();
    }

    for (Joint* j : this->destructJoints)
    {
        if (j->IsValid())
            j->DestroyJoint();

        // Release for reference in vector.
        j->Release();
    }

    this->destructBodies.clear();
    this->destructFixtures.clear();
    this->destructJoints.clear();

    if (this->destructWorld)
        this->Destroy();
}

void World::BeginContact(b2Contact* contact)
{
    this->begin.Process(contact);
}

void World::EndContact(b2Contact* bContact)
{
    this->end.Process(bContact);

    Contact* contact = (Contact*)this->FindObject(bContact);

    if (contact != nullptr)
        contact->Invalidate();
}

void World::PreSolve(b2Contact* contact, const b2Manifold* /*oldManifold*/)
{
    this->presolve.Process(contact);
}

void World::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    this->postsolve.Process(contact, impulse);
}

bool World::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
    // Fixtures should be memoized, if we created them
    Fixture* a = (Fixture*)this->FindObject(fixtureA);
    Fixture* b = (Fixture*)this->FindObject(fixtureB);

    if (!a || !b)
        throw love::Exception("A fixture has escaped Memoizer!");

    return filter.Process(a, b);
}

bool World::IsValid() const
{
    return this->world != nullptr;
}

int World::SetCallbacks(lua_State* L)
{
    int nargs = lua_gettop(L);

    for (int i = 1; i <= 4; i++)
    {
        if (!lua_isnoneornil(L, i))
            luaL_checktype(L, i, LUA_TFUNCTION);
    }

    delete this->begin.ref;
    this->begin.ref = nullptr;

    delete this->end.ref;
    this->end.ref = nullptr;

    delete this->presolve.ref;
    this->presolve.ref = nullptr;

    delete this->postsolve.ref;
    this->postsolve.ref = nullptr;

    if (nargs >= 1)
    {
        lua_pushvalue(L, 1);
        this->begin.ref = Luax::RefIf(L, LUA_TFUNCTION);
        this->begin.L   = L;
    }

    if (nargs >= 2)
    {
        lua_pushvalue(L, 2);
        this->end.ref = Luax::RefIf(L, LUA_TFUNCTION);
        this->end.L   = L;
    }

    if (nargs >= 3)
    {
        lua_pushvalue(L, 3);
        this->presolve.ref = Luax::RefIf(L, LUA_TFUNCTION);
        this->presolve.L   = L;
    }

    if (nargs >= 4)
    {
        lua_pushvalue(L, 4);
        this->postsolve.ref = Luax::RefIf(L, LUA_TFUNCTION);
        this->postsolve.L   = L;
    }

    return 0;
}

int World::GetCallbacks(lua_State* L)
{
    this->begin.ref ? this->begin.ref->Push(L) : lua_pushnil(L);
    this->end.ref ? this->end.ref->Push(L) : lua_pushnil(L);
    this->presolve.ref ? this->presolve.ref->Push(L) : lua_pushnil(L);
    this->postsolve.ref ? this->postsolve.ref->Push(L) : lua_pushnil(L);

    return 4;
}

void World::SetLuaCallbacks(lua_State* L)
{
    this->begin.L = this->end.L = this->presolve.L = this->postsolve.L = this->filter.L = L;
}

int World::SetContactFilter(lua_State* L)
{
    if (!lua_isnoneornil(L, 1))
        luaL_checktype(L, 1, LUA_TFUNCTION);

    if (this->filter.ref)
        delete this->filter.ref;

    this->filter.ref = Luax::RefIf(L, LUA_TFUNCTION);
    this->filter.L   = L;

    return 0;
}

int World::GetContactFilter(lua_State* L)
{
    this->filter.ref ? this->filter.ref->Push(L) : lua_pushnil(L);

    return 1;
}

void World::SetGravity(float x, float y)
{
    this->world->SetGravity(Physics::ScaleDown(b2Vec2(x, y)));
}

int World::GetGravity(lua_State* L)
{
    b2Vec2 v = Physics::ScaleUp(this->world->GetGravity());

    lua_pushnumber(L, v.x);
    lua_pushnumber(L, v.y);

    return 2;
}

void World::TranslateOrigin(float x, float y)
{
    this->world->ShiftOrigin(Physics::ScaleDown(b2Vec2(x, y)));
}

void World::SetSleepingAllowed(bool allow)
{
    this->world->SetAllowSleeping(allow);
}

bool World::IsSleepingAllowed() const
{
    return this->world->GetAllowSleeping();
}

bool World::IsLocked() const
{
    return this->world->IsLocked();
}

int World::GetBodyCount() const
{
    return this->world->GetBodyCount() - 1; // ignore the ground body
}

int World::GetJointCount() const
{
    return this->world->GetJointCount();
}

int World::GetContactCount() const
{
    return this->world->GetContactCount();
}

int World::GetBodies(lua_State* L) const
{
    lua_newtable(L);
    b2Body* bodyList = this->world->GetBodyList();

    int i = 1;

    do
    {
        if (!bodyList)
            break;

        if (bodyList == groundBody)
            continue;

        Body* body = (Body*)this->FindObject(bodyList);

        if (!body)
            throw love::Exception("A body has escaped Memoizer!");

        Luax::PushType(L, body);
        lua_rawseti(L, -2, i);

        i++;
    } while ((bodyList = bodyList->GetNext()));

    return 1;
}

int World::GetJoints(lua_State* L) const
{
    lua_newtable(L);
    b2Joint* jointList = this->world->GetJointList();

    int i = 1;

    do
    {
        if (!jointList)
            break;

        Joint* joint = (Joint*)this->FindObject(jointList);
        if (!joint)
            throw love::Exception("A joint has escaped Memoizer!");

        Wrap_Joint::PushJoint(L, joint);
        lua_rawseti(L, -2, i);

        i++;
    } while ((jointList = jointList->GetNext()));

    return 1;
}

int World::GetContacts(lua_State* L)
{
    lua_newtable(L);
    b2Contact* contactList = this->world->GetContactList();

    int i = 1;

    do
    {
        if (!contactList)
            break;

        Contact* contact = (Contact*)this->FindObject(contactList);

        if (!contact)
            contact = new Contact(this, contactList);
        else
            contact->Retain();

        Luax::PushType(L, contact);
        contact->Release();

        lua_rawseti(L, -2, i);

        i++;
    } while ((contactList = contactList->GetNext()));

    return 1;
}

b2Body* World::GetGroundBody() const
{
    return this->groundBody;
}

int World::QueryBoundingBox(lua_State* L)
{
    b2AABB box;
    float lx = (float)luaL_checknumber(L, 1);
    float ly = (float)luaL_checknumber(L, 2);
    float ux = (float)luaL_checknumber(L, 3);
    float uy = (float)luaL_checknumber(L, 4);

    box.lowerBound = Physics::ScaleDown(b2Vec2(lx, ly));
    box.upperBound = Physics::ScaleDown(b2Vec2(ux, uy));

    luaL_checktype(L, 5, LUA_TFUNCTION);
    QueryCallback query(this, L, 5);

    this->world->QueryAABB(&query, box);

    return 0;
}

int World::RayCast(lua_State* L)
{
    float x1 = (float)luaL_checknumber(L, 1);
    float y1 = (float)luaL_checknumber(L, 2);
    float x2 = (float)luaL_checknumber(L, 3);
    float y2 = (float)luaL_checknumber(L, 4);

    b2Vec2 v1 = Physics::ScaleDown(b2Vec2(x1, y1));
    b2Vec2 v2 = Physics::ScaleDown(b2Vec2(x2, y2));

    luaL_checktype(L, 5, LUA_TFUNCTION);
    RayCastCallback raycast(this, L, 5);

    this->world->RayCast(&raycast, v1, v2);

    return 0;
}

void World::Destroy()
{
    if (this->world == nullptr)
        return;

    if (this->world->IsLocked())
    {
        this->destructWorld = true;
        return;
    }

    // Remove userdata reference to avoid it sticking around after GC
    if (this->begin.ref)
        this->begin.ref->UnRef();

    if (this->end.ref)
        this->end.ref->UnRef();

    if (this->presolve.ref)
        this->presolve.ref->UnRef();

    if (this->postsolve.ref)
        this->postsolve.ref->UnRef();

    if (this->filter.ref)
        this->filter.ref->UnRef();

    // disable callbacks
    this->begin.ref = this->end.ref = this->presolve.ref = this->postsolve.ref = this->filter.ref =
        nullptr;

    // Cleaning up the world.
    b2Body* bodyList = world->GetBodyList();

    while (bodyList)
    {
        b2Body* bBody = bodyList;

        bodyList = bodyList->GetNext();
        if (bBody == groundBody)
            continue;

        Body* body = (Body*)this->FindObject(bBody);
        if (!body)
            throw love::Exception("A body has escaped Memoizer!");

        body->Destroy();
    }

    this->world->DestroyBody(this->groundBody);
    this->UnRegisterObject(this->world);

    delete this->world;
    this->world = nullptr;
}

void World::RegisterObject(void* b2object, love::Object* object)
{
    this->box2dObjectMap[b2object] = object;
}

void World::UnRegisterObject(void* b2object)
{
    this->box2dObjectMap.erase(b2object);
}

love::Object* World::FindObject(void* b2object) const
{
    auto it = this->box2dObjectMap.find(b2object);

    if (it != this->box2dObjectMap.end())
        return it->second;
    else
        return nullptr;
}
