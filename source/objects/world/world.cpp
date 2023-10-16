#include <objects/world/world.hpp>

#include <objects/contact/contact.hpp>
#include <objects/shape/shape.hpp>

#include <modules/physics/physics.hpp>

#include <objects/joint/wrap_joint.hpp>
#include <objects/shape/wrap_shape.hpp>

using namespace love;

Type World::type("World", &Object::type);

// #region ContactCallback

World::ContactCallback::ContactCallback(World* world) :
    reference(nullptr),
    state(nullptr),
    world(world)
{}

World::ContactCallback::~ContactCallback()
{
    if (this->reference != nullptr)
        delete this->reference;
}

void World::ContactCallback::Process(b2Contact* contact, const b2ContactImpulse* impulse)
{
    if (this->reference != nullptr && this->state != nullptr)
    {

        this->reference->Push(this->state);

        {
            auto* shapeA = (Shape*)contact->GetFixtureA()->GetUserData().pointer;

            if (shapeA == nullptr)
                throw love::Exception("A Shape has escaped Memoizer!");

            Wrap_Shape::PushShape(this->state, shapeA);
        }

        {
            auto* shapeB = (Shape*)contact->GetFixtureB()->GetUserData().pointer;

            if (shapeB == nullptr)
                throw love::Exception("A Shape has escaped Memoizer!");

            Wrap_Shape::PushShape(this->state, shapeB);
        }

        auto* newContact = (Contact*)this->world->FindObject(contact);
        if (newContact == nullptr)
            newContact = new Contact(this->world, contact);
        else
            newContact->Retain();

        luax::PushType(this->state, newContact);
        newContact->Release();

        int args = 3;
        if (impulse)
        {
            for (int count = 0; count < impulse->count; count++)
            {
                lua_pushnumber(this->state, impulse->normalImpulses[count]);
                lua_pushnumber(this->state, impulse->tangentImpulses[count]);
                args += 2;
            }
        }

        lua_call(this->state, args, 0);
    }
}

// #endregion ContactCallback

// #region ContactFilter

World::ContactFilter::ContactFilter() : reference(nullptr), state(nullptr)
{}

World::ContactFilter::~ContactFilter()
{
    if (this->reference != nullptr)
        delete this->reference;
}

bool World::ContactFilter::Process(Shape* a, Shape* b)
{
    if (this->reference != nullptr && this->state != nullptr)
    {
        this->reference->Push(this->state);

        Wrap_Shape::PushShape(this->state, a);
        Wrap_Shape::PushShape(this->state, b);

        lua_call(this->state, 2, 1);

        return luax::ToBoolean(this->state, -1);
    }

    return true;
}

// #endregion ContactFilter

// #region QueryCallback

World::QueryCallback::QueryCallback(World* world, lua_State* L, int index) :
    world(world),
    state(L),
    funcIndex(index)
{
    luaL_checktype(L, this->funcIndex, LUA_TFUNCTION);
    this->userArgs = lua_gettop(L) - this->funcIndex;
}

World::QueryCallback::~QueryCallback()
{}

bool World::QueryCallback::ReportFixture(b2Fixture* fixture)
{
    if (this->state != nullptr)
    {
        lua_pushvalue(this->state, this->funcIndex);

        auto* shape = (Shape*)fixture->GetUserData().pointer;

        if (!shape)
            throw love::Exception("A Shape has escaped Memoizer!");

        Wrap_Shape::PushShape(this->state, shape);

        for (int index = 1; index <= this->userArgs; index++)
            lua_pushvalue(this->state, this->funcIndex + index);

        lua_call(this->state, 1 + this->userArgs, 1);
        bool continueQuery = luax::ToBoolean(this->state, -1);
        lua_pop(this->state, 1);

        return continueQuery;
    }

    return true;
}

// #endregion QueryCallback

// #region CollectCallback

World::CollectCallback::CollectCallback(World* world, uint16_t categoryMask, lua_State* L) :
    world(world),
    categoryMask(categoryMask),
    state(L)
{
    lua_newtable(this->state);
}

World::CollectCallback::~CollectCallback()
{}

bool World::CollectCallback::ReportFixture(b2Fixture* fixture)
{
    const auto maskMax = 0xFFFF;
    if (categoryMask != maskMax && (fixture->GetFilterData().categoryBits & categoryMask) == 0)
        return true;

    auto* shape = (Shape*)fixture->GetUserData().pointer;

    if (!shape)
        throw love::Exception("A Shape has escaped Memoizer!");

    Wrap_Shape::PushShape(this->state, shape);
    lua_rawseti(this->state, -2, this->index);
    this->index++;

    return true;
}

// #endregion CollectCallback

// #region RayCastCallback

World::RayCastCallback::RayCastCallback(World* world, lua_State* L, int index) :
    world(world),
    state(L),
    funcIndex(index)
{
    luaL_checktype(L, this->funcIndex, LUA_TFUNCTION);
    this->userArgs = lua_gettop(L) - this->funcIndex;
}

World::RayCastCallback::~RayCastCallback()
{}

float World::RayCastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point,
                                            const b2Vec2& normal, float fraction)
{
    if (this->state != nullptr)
    {
        lua_pushvalue(this->state, this->funcIndex);

        auto* shape = (Shape*)fixture->GetUserData().pointer;

        if (!shape)
            throw love::Exception("A Shape has escaped Memoizer!");

        Wrap_Shape::PushShape(this->state, shape);

        b2Vec2 scaledPoint = Physics::ScaleUp(point);

        lua_pushnumber(this->state, scaledPoint.x);
        lua_pushnumber(this->state, scaledPoint.y);

        lua_pushnumber(this->state, normal.x);
        lua_pushnumber(this->state, normal.y);

        lua_pushnumber(this->state, fraction);

        for (int index = 1; index <= this->userArgs; index++)
            lua_pushvalue(this->state, this->funcIndex + index);

        lua_call(this->state, 6 + this->userArgs, 1);

        if (!lua_isnumber(this->state, -1))
            luaL_error(this->state, "Raycast callback didn't return a number!");

        float fraction = lua_tonumber(this->state, -1);
        lua_pop(this->state, 1);

        return fraction;
    }

    return 0.0f;
}

// #endregion RayCastCallback

// #region RayCastOneCallback

World::RayCastOneCallback::RayCastOneCallback(uint16_t categoryMask, bool any) :
    hitFixture(nullptr),
    hitPoint {},
    hitNormal {},
    hitFraction(1.0f),
    categoryMask(categoryMask),
    any(any)
{}

float World::RayCastOneCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point,
                                               const b2Vec2& normal, float fraction)
{
    const auto maxMask = 0xFFFF;
    if (this->categoryMask != maxMask &&
        (fixture->GetFilterData().categoryBits & this->categoryMask) == 0)
    {
        return -1.0f;
    }

    this->hitFixture  = fixture;
    this->hitPoint    = point;
    this->hitNormal   = normal;
    this->hitFraction = fraction;

    return this->any ? 0.0f : fraction;
}

// #endregion RayCastOneCallback

World::World() :
    world(nullptr),
    destructWorld(false),
    begin(this),
    end(this),
    preSolve(this),
    postSolve(this)
{
    this->world = new b2World(b2Vec2(0.0f, 0.0f));
    this->world->SetAllowSleeping(true);
    this->world->SetContactListener(this);
    this->world->SetContactFilter(this);
    this->world->SetDestructionListener(this);

    b2BodyDef bodyDef {};
    this->groundBody = this->world->CreateBody(&bodyDef);

    this->RegisterObject(this->world, this);
}

World::World(b2Vec2 gravity, bool sleep) :
    world(nullptr),
    destructWorld(false),
    begin(this),
    end(this),
    preSolve(this),
    postSolve(this)
{
    this->world = new b2World(Physics::ScaleDown(gravity));
    this->world->SetAllowSleeping(sleep);
    this->world->SetContactListener(this);
    this->world->SetContactFilter(this);
    this->world->SetDestructionListener(this);

    b2BodyDef bodyDef {};
    this->groundBody = this->world->CreateBody(&bodyDef);

    this->RegisterObject(this->world, this);
}

World::~World()
{
    this->Destroy();
}

void World::SayGoodbye(b2Fixture* fixture)
{
    auto* shape = (Shape*)fixture->GetUserData().pointer;

    if (shape)
        shape->Destroy(true);
}

void World::SayGoodbye(b2Joint* joint)
{
    auto* _joint = (Joint*)joint->GetUserData().pointer;

    if (_joint)
        _joint->DestroyJoint(true);
}

void World::Update(float delta)
{
    this->Update(delta, 8, 3);
}

void World::Update(float delta, int velocityIterations, int positionIterations)
{
    this->world->Step(delta, velocityIterations, positionIterations);

    for (auto* body : this->destructBodies)
    {
        if (body->body != nullptr)
            body->Destroy();

        body->Release();
    }

    for (auto* shape : this->destructShapes)
    {
        if (shape->IsValid())
            shape->Destroy();

        shape->Release();
    }

    for (auto* joint : this->destructJoints)
    {
        if (joint->IsValid())
            joint->DestroyJoint();

        joint->Release();
    }

    this->destructBodies.clear();
    this->destructShapes.clear();
    this->destructJoints.clear();

    if (this->destructWorld)
        this->Destroy();
}

void World::BeginContact(b2Contact* contact)
{
    this->begin.Process(contact, nullptr);
}

void World::EndContact(b2Contact* contact)
{
    this->end.Process(contact, nullptr);

    auto* _contact = (Contact*)this->FindObject(contact);
    if (_contact != nullptr)
        _contact->Invalidate();
}

void World::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    B2_NOT_USED(oldManifold);
    this->preSolve.Process(contact);
}

void World::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    this->postSolve.Process(contact, impulse);
}

bool World::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
    const auto& filterA = fixtureA->GetFilterData();
    const auto& filterB = fixtureB->GetFilterData();

    if (filterA.groupIndex != 0 && filterA.groupIndex == filterB.groupIndex)
        return filterA.groupIndex > 0;

    const auto disallowFilterA = filterA.maskBits & filterB.categoryBits;
    const auto disallowFilterB = filterA.categoryBits & filterB.maskBits;

    if (disallowFilterA == 0 || disallowFilterB == 0)
        return false;

    auto* shapeA = (Shape*)fixtureA->GetUserData().pointer;
    auto* shapeB = (Shape*)fixtureB->GetUserData().pointer;

    if (shapeA == nullptr || shapeB == nullptr)
        throw love::Exception("A Shape has escaped Memoizer!");

    return this->filter.Process(shapeA, shapeB);
}

bool World::IsValid() const
{
    return this->world != nullptr;
}

int World::SetCallbacks(lua_State* L)
{
    const auto args = lua_gettop(L);

    for (int index = 1; index <= 4; index++)
    {
        if (!lua_isnoneornil(L, index))
            luaL_checktype(L, index, LUA_TFUNCTION);
    }

    delete this->begin.reference;
    this->begin.reference = nullptr;

    delete this->end.reference;
    this->end.reference = nullptr;

    delete this->preSolve.reference;
    this->preSolve.reference = nullptr;

    delete this->postSolve.reference;
    this->postSolve.reference = nullptr;

    if (args >= 1)
    {
        lua_pushvalue(L, 1);
        this->begin.reference = luax::RefIfType(L, LUA_TFUNCTION);
        this->begin.state     = L;
    }

    if (args >= 2)
    {
        lua_pushvalue(L, 2);
        this->end.reference = luax::RefIfType(L, LUA_TFUNCTION);
        this->end.state     = L;
    }

    if (args >= 3)
    {
        lua_pushvalue(L, 3);
        this->preSolve.reference = luax::RefIfType(L, LUA_TFUNCTION);
        this->preSolve.state     = L;
    }

    if (args >= 4)
    {
        lua_pushvalue(L, 4);
        this->postSolve.reference = luax::RefIfType(L, LUA_TFUNCTION);
        this->postSolve.state     = L;
    }

    return 0;
}

int World::GetCallbacks(lua_State* L)
{
    this->begin.reference ? this->begin.reference->Push(L) : lua_pushnil(L);
    this->end.reference ? this->end.reference->Push(L) : lua_pushnil(L);
    this->preSolve.reference ? this->preSolve.reference->Push(L) : lua_pushnil(L);
    this->postSolve.reference ? this->postSolve.reference->Push(L) : lua_pushnil(L);

    return 4;
}

void World::SetCallbacksL(lua_State* L)
{
    this->begin.state = this->end.state = this->preSolve.state = this->postSolve.state = L;
}

int World::SetContactFilter(lua_State* L)
{
    if (!lua_isnoneornil(L, 1))
        luaL_checktype(L, 1, LUA_TFUNCTION);

    if (this->filter.reference)
        delete this->filter.reference;

    this->filter.reference = luax::RefIfType(L, LUA_TFUNCTION);
    this->filter.state     = L;

    return 0;
}

int World::GetContactFilter(lua_State* L)
{
    this->filter.reference ? this->filter.reference->Push(L) : lua_pushnil(L);

    return 1;
}

void World::SetGravity(float x, float y)
{
    this->world->SetGravity(Physics::ScaleDown(b2Vec2(x, y)));
}

int World::GetGravity(lua_State* L)
{
    const auto& gravity = Physics::ScaleUp(this->world->GetGravity());
    lua_pushnumber(L, gravity.x);
    lua_pushnumber(L, gravity.y);

    return 2;
}

void World::TranslateOrigin(float x, float y)
{
    this->world->ShiftOrigin(Physics::ScaleDown(b2Vec2(x, y)));
}

void World::SetSleepingAllowed(bool allowed)
{
    this->world->SetAllowSleeping(allowed);
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
    return this->world->GetBodyCount() - 1;
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

    auto* bodies = this->world->GetBodyList();
    int index    = 1;

    do
    {
        if (!bodies)
            break;

        if (bodies == this->groundBody)
            continue;

        auto* body = (Body*)bodies->GetUserData().pointer;

        if (!body)
            throw love::Exception("A Body has escaped Memoizer!");

        luax::PushType(L, body);
        lua_rawseti(L, -2, index);
        index++;
    } while ((bodies = bodies->GetNext()));

    return 1;
}

int World::GetJoints(lua_State* L) const
{
    lua_newtable(L);

    auto* joints = this->world->GetJointList();
    int index    = 1;

    do
    {
        if (!joints)
            break;

        auto* joint = (Joint*)joints->GetUserData().pointer;

        if (!joint)
            throw love::Exception("A Joint has escaped Memoizer!");

        luax::PushType(L, joint);
        lua_rawseti(L, -2, index);
        index++;
    } while ((joints = joints->GetNext()));

    return 1;
}

int World::GetContacts(lua_State* L)
{
    lua_newtable(L);

    auto* contacts = this->world->GetContactList();
    int index      = 1;

    do
    {
        if (!contacts)
            break;

        auto* contact = (Contact*)this->FindObject(contacts);

        if (!contact)
            contact = new Contact(this, contacts);
        else
            contact->Retain();

        luax::PushType(L, contact);
        contact->Release();
        lua_rawseti(L, -2, index);
        index++;
    } while ((contacts = contacts->GetNext()));

    return 1;
}

b2Body* World::GetGroundBody() const
{
    return this->groundBody;
}

int World::QueryShapesInArea(lua_State* L)
{
    b2AABB aabb {};

    float lowerX = luaL_checknumber(L, 1);
    float lowerY = luaL_checknumber(L, 2);
    float upperX = luaL_checknumber(L, 3);
    float upperY = luaL_checknumber(L, 4);

    aabb.lowerBound = Physics::ScaleDown(b2Vec2(lowerX, lowerY));
    aabb.upperBound = Physics::ScaleDown(b2Vec2(upperX, upperY));

    luaL_checktype(L, 5, LUA_TFUNCTION);

    QueryCallback query(this, L, 5);
    this->world->QueryAABB(&query, aabb);

    return 0;
}

int World::GetShapesInArea(lua_State* L)
{
    float lowerX              = luaL_checknumber(L, 1);
    float lowerY              = luaL_checknumber(L, 2);
    float upperX              = luaL_checknumber(L, 3);
    float upperY              = luaL_checknumber(L, 4);
    uint16_t categoryMaskBits = luaL_optinteger(L, 5, 0xFFFF);

    b2AABB aabb {};
    aabb.lowerBound = Physics::ScaleDown(b2Vec2(lowerX, lowerY));
    aabb.upperBound = Physics::ScaleDown(b2Vec2(upperX, upperY));

    CollectCallback query(this, categoryMaskBits, L);
    this->world->QueryAABB(&query, aabb);

    return 1;
}

int World::RayCast(lua_State* L)
{
    float x1 = luaL_checknumber(L, 1);
    float y1 = luaL_checknumber(L, 2);
    float x2 = luaL_checknumber(L, 3);
    float y2 = luaL_checknumber(L, 4);

    const auto firstRayCast  = Physics::ScaleDown(b2Vec2(x1, y1));
    const auto secondRayCast = Physics::ScaleDown(b2Vec2(x2, y2));

    luaL_checktype(L, 5, LUA_TFUNCTION);

    RayCastCallback rayCast(this, L, 5);
    this->world->RayCast(&rayCast, firstRayCast, secondRayCast);

    return 0;
}

int World::RayCastAny(lua_State* L)
{
    float x1 = luaL_checknumber(L, 1);
    float y1 = luaL_checknumber(L, 2);
    float x2 = luaL_checknumber(L, 3);
    float y2 = luaL_checknumber(L, 4);

    uint16_t categoryMaskBits = luaL_optinteger(L, 5, 0xFFFF);

    const auto firstRayCast  = Physics::ScaleDown(b2Vec2(x1, y1));
    const auto secondRayCast = Physics::ScaleDown(b2Vec2(x2, y2));

    RayCastOneCallback rayCast(categoryMaskBits, true);
    this->world->RayCast(&rayCast, firstRayCast, secondRayCast);

    if (rayCast.hitFixture)
    {
        auto* shape = (Shape*)rayCast.hitFixture->GetUserData().pointer;

        if (shape == nullptr)
            return luaL_error(L, "A Shape has escaped Memoizer!");

        Wrap_Shape::PushShape(L, shape);

        const auto hitPoint = Physics::ScaleUp(rayCast.hitPoint);

        lua_pushnumber(L, hitPoint.x);
        lua_pushnumber(L, hitPoint.y);
        lua_pushnumber(L, rayCast.hitNormal.x);
        lua_pushnumber(L, rayCast.hitNormal.y);
        lua_pushnumber(L, rayCast.hitFraction);

        return 6;
    }

    return 0;
}

int World::RayCastClosest(lua_State* L)
{
    float x1 = luaL_checknumber(L, 1);
    float y1 = luaL_checknumber(L, 2);
    float x2 = luaL_checknumber(L, 3);
    float y2 = luaL_checknumber(L, 4);

    uint16_t categoryMaskBits = luaL_optinteger(L, 5, 0xFFFF);

    const auto firstRayCast  = Physics::ScaleDown(b2Vec2(x1, y1));
    const auto secondRayCast = Physics::ScaleDown(b2Vec2(x2, y2));

    RayCastOneCallback rayCast(categoryMaskBits, false);
    this->world->RayCast(&rayCast, firstRayCast, secondRayCast);

    if (rayCast.hitFixture)
    {
        auto* shape = (Shape*)rayCast.hitFixture->GetUserData().pointer;

        if (shape == nullptr)
            return luaL_error(L, "A Shape has escaped Memoizer!");

        Wrap_Shape::PushShape(L, shape);

        const auto hitPoint = Physics::ScaleUp(rayCast.hitPoint);

        lua_pushnumber(L, hitPoint.x);
        lua_pushnumber(L, hitPoint.y);
        lua_pushnumber(L, rayCast.hitNormal.x);
        lua_pushnumber(L, rayCast.hitNormal.y);
        lua_pushnumber(L, rayCast.hitFraction);

        return 6;
    }

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

    if (this->begin.reference)
        this->begin.reference->UnReference();

    if (this->end.reference)
        this->end.reference->UnReference();

    if (this->preSolve.reference)
        this->preSolve.reference->UnReference();

    if (this->postSolve.reference)
        this->postSolve.reference->UnReference();

    if (this->filter.reference)
        this->filter.reference->UnReference();

    this->begin.reference = this->end.reference = nullptr;
    this->preSolve.reference = this->postSolve.reference = nullptr;
    this->filter.reference                               = nullptr;

    auto* bodies = this->world->GetBodyList();

    while (bodies)
    {
        auto* tail = bodies;
        bodies     = bodies->GetNext();

        if (tail == this->groundBody)
            continue;

        auto* body = (Body*)tail->GetUserData().pointer;

        if (!body)
            throw love::Exception("A Body has escaped Memoizer!");

        body->Destroy();
    }

    this->world->DestroyBody(this->groundBody);
    this->UnregisterObject(this->world);

    delete this->world;
    this->world = nullptr;
}

void World::RegisterObject(void* b2Object, Object* object)
{
    this->box2dObjectMap[b2Object] = object;
}

void World::UnregisterObject(void* b2Object)
{
    this->box2dObjectMap.erase(b2Object);
}

Object* World::FindObject(void* b2Object) const
{
    auto it = this->box2dObjectMap.find(b2Object);

    if (it == this->box2dObjectMap.end())
        return nullptr;

    return it->second;
}
