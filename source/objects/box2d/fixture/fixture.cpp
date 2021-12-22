#include "objects/box2d/fixture/fixture.h"
#include "modules/physics/physics.h"

#include "world.h"

#include <bitset>

using namespace love;

love::Type Fixture::type("Fixture", &Object::type);

Fixture::Fixture(Body* body, Shape* shape, float density) : body(body), fixture(nullptr)
{
    this->userdata = new FixtureUserdata { .ref = nullptr };

    b2FixtureDef fixtureDefinition;
    fixtureDefinition.shape            = shape->shape;
    fixtureDefinition.userData.pointer = (uintptr_t)this->userdata;
    fixtureDefinition.density          = density;

    this->fixture = body->body->CreateFixture(&fixtureDefinition);
    this->Retain();

    this->body->world->RegisterObject(this->fixture, this);
}

Fixture::~Fixture()
{
    if (!this->userdata)
        return;

    if (this->userdata->ref)
        delete this->userdata->ref;

    delete this->userdata;
}

void Fixture::CheckCreateShape()
{
    if (this->shape.Get() != nullptr || this->fixture == nullptr ||
        this->fixture->GetShape() == nullptr)
        return;

    b2Shape* bShape = this->fixture->GetShape();

    switch (bShape->GetType())
    {
        case b2Shape::e_circle:
            this->shape.Set(new CircleShape((b2CircleShape*)bShape, false), Acquire::NORETAIN);
            break;
        case b2Shape::e_edge:
            this->shape.Set(new EdgeShape((b2EdgeShape*)bShape, false), Acquire::NORETAIN);
            break;
        case b2Shape::e_polygon:
            this->shape.Set(new PolygonShape((b2PolygonShape*)bShape, false), Acquire::NORETAIN);
            break;
        case b2Shape::e_chain:
            this->shape.Set(new ChainShape((b2ChainShape*)bShape, false), Acquire::NORETAIN);
            break;
        default:
            break;
    }
}

Shape::Type Fixture::GetType()
{
    this->CheckCreateShape();

    if (this->shape.Get() == nullptr)
        return Shape::SHAPE_INVALID;

    return this->shape->GetType();
}

void Fixture::SetFriction(float friction)
{
    this->fixture->SetFriction(friction);
}

void Fixture::SetRestitution(float restitution)
{
    this->fixture->SetRestitution(restitution);
}

void Fixture::SetDensity(float density)
{
    this->fixture->SetDensity(density);
}

void Fixture::SetSensor(bool sensor)
{
    this->fixture->SetSensor(sensor);
}

float Fixture::GetFriction() const
{
    return this->fixture->GetFriction();
}

float Fixture::GetRestitution() const
{
    return this->fixture->GetRestitution();
}

float Fixture::GetDensity() const
{
    return this->fixture->GetDensity();
}

bool Fixture::IsSensor() const
{
    return this->fixture->IsSensor();
}

Body* Fixture::GetBody() const
{
    return this->body;
}

Shape* Fixture::GetShape()
{
    this->CheckCreateShape();

    return this->shape;
}

bool Fixture::IsValid() const
{
    return this->fixture != nullptr;
}

void Fixture::SetFilterData(int* filter)
{
    b2Filter bFilter;

    bFilter.categoryBits = (uint16_t)filter[0];
    bFilter.maskBits     = (uint16_t)filter[1];
    bFilter.groupIndex   = (uint16_t)filter[2];

    this->fixture->SetFilterData(bFilter);
}

void Fixture::GetFilterData(int* filter)
{
    b2Filter bFilter = this->fixture->GetFilterData();

    filter[0] = (int)bFilter.categoryBits;
    filter[1] = (int)bFilter.maskBits;
    filter[2] = (int)bFilter.groupIndex;
}

int Fixture::SetCategory(lua_State* L)
{
    b2Filter bFilter = this->fixture->GetFilterData();

    bFilter.categoryBits = (uint16_t)this->GetBits(L);
    this->fixture->SetFilterData(bFilter);

    return 0;
}

int Fixture::SetMask(lua_State* L)
{
    b2Filter bFilter = this->fixture->GetFilterData();

    bFilter.maskBits = ~(uint16_t)this->GetBits(L);
    this->fixture->SetFilterData(bFilter);

    return 0;
}

void Fixture::SetGroupIndex(int index)
{
    b2Filter bFilter = this->fixture->GetFilterData();

    bFilter.groupIndex = (uint16_t)index;
    this->fixture->SetFilterData(bFilter);
}

int Fixture::GetGroupIndex() const
{
    b2Filter bFilter = this->fixture->GetFilterData();

    return bFilter.groupIndex;
}

int Fixture::GetCategory(lua_State* L)
{
    return this->PushBits(L, this->fixture->GetFilterData().categoryBits);
}

int Fixture::GetMask(lua_State* L)
{
    return this->PushBits(L, ~(this->fixture->GetFilterData().maskBits));
}

uint16_t Fixture::GetBits(lua_State* L)
{
    /* Get number of args */
    bool isTable = lua_istable(L, 1);
    int argc     = isTable ? lua_objlen(L, 1) : lua_gettop(L);

    std::bitset<16> bits;

    for (int index = 1; index <= argc; index++)
    {
        size_t bitPosition = 0;

        if (isTable)
        {
            lua_rawgeti(L, 1, index);
            bitPosition = lua_tointeger(L, -1) - 1;
            lua_pop(L, 1);
        }
        else
            bitPosition = lua_tointeger(L, index) - 1;

        if (bitPosition >= 16)
            luaL_error(L, "Values must be in range [1 - 16].");

        bits.set(bitPosition, true);
    }

    return (uint16_t)bits.to_ulong();
}

bool Fixture::TestPoint(float x, float y) const
{
    return this->fixture->TestPoint(Physics::ScaleDown(b2Vec2(x, y)));
}

int Fixture::PushBits(lua_State* L, uint16_t bits)
{
    std::bitset<16> bset((int)bits);

    for (int index = 0; index < 16; index++)
    {
        if (bset.test(index))
            lua_pushinteger(L, index + 1);
    }

    return (int)bset.count();
}

int Fixture::SetUserdata(lua_State* L)
{
    if (this->userdata == nullptr)
    {
        this->userdata                 = new FixtureUserdata();
        fixture->GetUserData().pointer = (uintptr_t)this->userdata;
    }

    if (!this->userdata->ref)
        this->userdata->ref = new Reference();

    this->userdata->ref->Ref(L);

    return 0;
}

int Fixture::GetUserdata(lua_State* L)
{
    if (this->userdata->ref != nullptr)
        this->userdata->ref->Push(L);
    else
        lua_pushnil(L);

    return 1;
}

int Fixture::RayCast(lua_State* L) const
{
    float pointA = Physics::ScaleDown(luaL_checknumber(L, 1));
    float pointB = Physics::ScaleDown(luaL_checknumber(L, 2));
    float pointC = Physics::ScaleDown(luaL_checknumber(L, 3));
    float pointD = Physics::ScaleDown(luaL_checknumber(L, 4));

    float maxFriction = luaL_checknumber(L, 5);
    int childIndex    = luaL_optinteger(L, 6, 1) - 1;

    b2RayCastInput input;
    input.p1.Set(pointA, pointB);
    input.p2.Set(pointC, pointD);
    input.maxFraction = maxFriction;

    b2RayCastOutput output;

    if (!this->fixture->RayCast(&output, input, childIndex))
        return 0;

    lua_pushnumber(L, output.normal.x);
    lua_pushnumber(L, output.normal.y);
    lua_pushnumber(L, output.fraction);

    return 3;
}

int Fixture::GetBoundingBox(lua_State* L) const
{
    int childIndex = luaL_optinteger(L, 1, 1) - 1;

    b2AABB box;

    Luax::CatchException(L, [&]() { box = this->fixture->GetAABB(childIndex); });

    box = Physics::ScaleUp(box);

    lua_pushnumber(L, box.lowerBound.x);
    lua_pushnumber(L, box.lowerBound.y);
    lua_pushnumber(L, box.upperBound.x);
    lua_pushnumber(L, box.upperBound.y);

    return 4;
}

int Fixture::GetMassData(lua_State* L) const
{
    b2MassData data;
    this->fixture->GetMassData(&data);

    b2Vec2 center = Physics::ScaleUp(data.center);

    lua_pushnumber(L, center.x);
    lua_pushnumber(L, center.y);
    lua_pushnumber(L, data.mass);
    lua_pushnumber(L, data.I);

    return 4;
}

void Fixture::Destroy(bool implicit)
{
    if (this->body->world->world->IsLocked())
    {
        this->Retain();
        this->body->world->destructFixtures.push_back(this);

        return;
    }

    this->shape.Set(nullptr);

    if (!implicit && this->fixture != nullptr)
        this->body->body->DestroyFixture(this->fixture);

    this->body->world->UnRegisterObject(this->fixture);
    this->fixture = nullptr;

    if (this->userdata && this->userdata->ref)
        this->userdata->ref->UnRef();

    this->Release();
}
