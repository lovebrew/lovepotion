#include <objects/shape/shape.hpp>

#include <modules/physics/physics.hpp>

#include <objects/world/world.hpp>

using namespace love;

Type Shape::type("Shape", &Object::type);

Shape::Shape(Body* body, const b2Shape& shape) :
    shape(nullptr),
    own(false),
    shapeType(SHAPE_INVALID),
    body(body),
    fixture(nullptr)
{
    if (body != nullptr)
    {
        b2FixtureDef definition {};
        definition.shape            = &shape;
        definition.userData.pointer = (uintptr_t)this;
        definition.density          = body->HasCustomMassData() ? 0.0f : 1.0f;

        this->fixture = body->body->CreateFixture(&definition);
        this->shape   = this->fixture->GetShape();

        if (body->HasCustomMassData())
            this->SetDensity(1.0f);

        this->Retain();
    }
    else
    {
        auto physics = Module::GetInstance<Physics>(Module::M_PHYSICS);
        this->shape  = shape.Clone(physics->GetAllocator());
        this->own    = true;
    }

    switch (this->shape->GetType())
    {
        case b2Shape::e_circle:
        {
            this->shapeType = SHAPE_CIRCLE;
            break;
        }
        case b2Shape::e_polygon:
        {
            this->shapeType = SHAPE_POLYGON;
            break;
        }
        case b2Shape::e_edge:
        {
            this->shapeType = SHAPE_EDGE;
            break;
        }
        case b2Shape::e_chain:
        {
            this->shapeType = SHAPE_CHAIN;
            break;
        }
        default:
        {
            this->shapeType = SHAPE_INVALID;
            break;
        }
    }
}

Shape::~Shape()
{
    if (this->shape && this->own)
    {
        auto physics    = Module::GetInstance<Physics>(Module::M_PHYSICS);
        auto* allocator = physics->GetAllocator();

        switch (this->shapeType)
        {
            case SHAPE_CIRCLE:
            {
                auto* circle = (b2CircleShape*)this->shape;
                circle->~b2CircleShape();

                allocator->Free(circle, sizeof(b2CircleShape));
                break;
            }
            case SHAPE_POLYGON:
            {
                auto* polygon = (b2PolygonShape*)this->shape;
                polygon->~b2PolygonShape();

                allocator->Free(polygon, sizeof(b2PolygonShape));
                break;
            }
            case SHAPE_EDGE:
            {
                auto* edge = (b2EdgeShape*)this->shape;
                edge->~b2EdgeShape();

                allocator->Free(edge, sizeof(b2EdgeShape));
                break;
            }
            case SHAPE_CHAIN:
            {
                auto* chain = (b2ChainShape*)this->shape;
                chain->~b2ChainShape();

                allocator->Free(chain, sizeof(b2ChainShape));
                break;
            }
            default:
                break;
        }
    }

    if (this->reference)
        delete this->reference;
}

void Shape::Destroy(bool implicit)
{
    if (this->fixture == nullptr)
        return;

    if (this->body->world->world->IsLocked())
    {
        this->Retain();
        this->body->world->destructShapes.push_back(this);
        return;
    }

    if (!implicit && this->fixture != nullptr)
        this->body->body->DestroyFixture(fixture);

    this->fixture = nullptr;
    this->body    = nullptr;
    this->shape   = nullptr;

    if (this->reference)
        this->reference->UnReference();

    this->Release();
}

void Shape::ThrowIfFixtureNotValid() const
{
    if (this->fixture == nullptr)
        throw love::Exception("Shape must be active in the physics World to use this method.");
}

void Shape::ThrowIfShapeNotValid() const
{
    if (this->shape == nullptr)
        throw love::Exception("Cannot call this method on a destroyed Shape.");
}

Shape::Type Shape::GetType() const
{
    return this->shapeType;
}

void Shape::SetFriction(float friction)
{
    this->ThrowIfFixtureNotValid();
    this->fixture->SetFriction(friction);
}

void Shape::SetRestitution(float restitution)
{
    this->ThrowIfFixtureNotValid();
    this->fixture->SetRestitution(restitution);
}

void Shape::SetDensity(float density)
{
    this->ThrowIfFixtureNotValid();
    this->fixture->SetDensity(density);

    if (!this->body->HasCustomMassData())
        this->body->ResetMassData();
}

void Shape::SetSensor(bool sensor)
{
    this->ThrowIfFixtureNotValid();
    this->fixture->SetSensor(sensor);
}

float Shape::GetFriction() const
{
    this->ThrowIfFixtureNotValid();
    return this->fixture->GetFriction();
}

float Shape::GetRestitution() const
{
    this->ThrowIfFixtureNotValid();
    return this->fixture->GetRestitution();
}

float Shape::GetDensity() const
{
    this->ThrowIfFixtureNotValid();
    return this->fixture->GetDensity();
}

bool Shape::IsSensor() const
{
    this->ThrowIfFixtureNotValid();
    return this->fixture->IsSensor();
}

Body* Shape::GetBody() const
{
    return this->body;
}

float Shape::GetRadius() const
{
    this->ThrowIfShapeNotValid();
    return Physics::ScaleUp(this->shape->m_radius);
}

int Shape::GetChildCount() const
{
    this->ThrowIfShapeNotValid();
    return this->shape->GetChildCount();
}

void Shape::SetFilterData(int* data)
{
    this->ThrowIfFixtureNotValid();

    b2Filter filter {};
    filter.categoryBits = (uint16_t)data[0];
    filter.maskBits     = (uint16_t)data[1];
    filter.groupIndex   = (uint16_t)data[2];

    this->fixture->SetFilterData(filter);
}

void Shape::GetFilterData(int* data)
{
    this->ThrowIfFixtureNotValid();

    auto filter = this->fixture->GetFilterData();
    data[0]     = (int)filter.categoryBits;
    data[1]     = (int)filter.maskBits;
    data[2]     = (int)filter.groupIndex;
}

int Shape::SetCategory(lua_State* L)
{
    this->ThrowIfFixtureNotValid();

    auto filter         = this->fixture->GetFilterData();
    filter.categoryBits = (uint16_t)this->GetCategoryBits(L);
    this->fixture->SetFilterData(filter);

    return 0;
}

int Shape::SetMask(lua_State* L)
{
    this->ThrowIfFixtureNotValid();

    auto filter     = this->fixture->GetFilterData();
    filter.maskBits = ~(uint16_t)this->GetCategoryBits(L);
    this->fixture->SetFilterData(filter);

    return 0;
}

void Shape::SetGroupIndex(int index)
{
    this->ThrowIfFixtureNotValid();

    auto filter       = this->fixture->GetFilterData();
    filter.groupIndex = (uint16_t)index;
    this->fixture->SetFilterData(filter);
}

int Shape::GetGroupIndex() const
{
    this->ThrowIfFixtureNotValid();
    return (int)this->fixture->GetFilterData().groupIndex;
}

int Shape::GetCategory(lua_State* L)
{
    this->ThrowIfFixtureNotValid();

    const auto bits = this->fixture->GetFilterData().categoryBits;
    return this->PushCategoryBits(L, bits);
}

int Shape::GetMask(lua_State* L)
{
    this->ThrowIfFixtureNotValid();

    const auto bits = this->fixture->GetFilterData().maskBits;
    return this->PushCategoryBits(L, ~bits);
}

uint16_t Shape::GetCategoryBits(lua_State* L) const
{
    const auto isTable = lua_istable(L, -1);
    const auto size    = isTable ? luax::ObjectLength(L, 1) : 1;

    std::bitset<0x10> bits;

    for (int index = 0; index <= size; index++)
    {
        size_t position = 0;

        if (isTable)
        {
            lua_rawgeti(L, 1, index);
            position = (size_t)(lua_tointeger(L, -1) - 1);
            lua_pop(L, 1);
        }
        else
            position = (size_t)(lua_tointeger(L, index) - 1);

        if (position >= bits.size())
            luaL_error(L, "Values must be in range 1-16.");

        bits.set(position, true);
    }

    return (uint16_t)bits.to_ulong();
}

int Shape::PushCategoryBits(lua_State* L, uint16_t bits)
{
    std::bitset<0x10> bitset((int)bits);

    for (int index = 0; index < bitset.size(); index++)
    {
        if (bitset.test(index))
            lua_pushinteger(L, index + 1);
    }

    return (int)bitset.count();
}

int Shape::SetUserdata(lua_State* L)
{
    luax::AssertArgCount<1, 1>(L);

    if (this->reference == nullptr)
        this->reference = new Reference();

    this->reference->Create(L);
    return 0;
}

int Shape::GetUserdata(lua_State* L)
{
    if (this->reference != nullptr)
        this->reference->Push(L);
    else
        lua_pushnil(L);

    return 1;
}

bool Shape::TestPoint(float x, float y) const
{
    this->ThrowIfFixtureNotValid();

    const auto point = b2Vec2(Physics::ScaleDown(x), Physics::ScaleDown(y));
    return this->fixture->TestPoint(point);
}

bool Shape::TestPoint(float x, float y, float angle, float cx, float cy) const
{
    this->ThrowIfShapeNotValid();

    const auto point = b2Vec2(Physics::ScaleDown(cx), Physics::ScaleDown(cy));
    b2Transform transform(Physics::ScaleDown(b2Vec2(x, y)), b2Rot(angle));

    return this->shape->TestPoint(transform, Physics::ScaleDown(point));
}

int Shape::RayCast(lua_State* L) const
{
    float point1x     = Physics::ScaleDown(luaL_checknumber(L, 1));
    float point1y     = Physics::ScaleDown(luaL_checknumber(L, 2));
    float point2x     = Physics::ScaleDown(luaL_checknumber(L, 3));
    float point2y     = Physics::ScaleDown(luaL_checknumber(L, 4));
    float maxFraction = luaL_checknumber(L, 5);

    b2RayCastInput input {};
    b2RayCastOutput output {};

    input.p1.Set(point1x, point1y);
    input.p2.Set(point2x, point2y);
    input.maxFraction = maxFraction;

    if (lua_isnoneornil(L, 7))
    {
        this->ThrowIfFixtureNotValid();
        int index = (int)luaL_optinteger(L, 6, 1) - 1;

        if (!this->fixture->RayCast(&output, input, index))
            return 0;
    }
    else
    {
        this->ThrowIfShapeNotValid();

        float x     = Physics::ScaleDown(luaL_checknumber(L, 6));
        float y     = Physics::ScaleDown(luaL_checknumber(L, 7));
        float angle = luaL_checknumber(L, 8);
        int index   = luaL_optinteger(L, 9, 1) - 1;

        b2Transform transform(b2Vec2(x, y), b2Rot(angle));

        if (!shape->RayCast(&output, input, transform, index))
            return 0;
    }

    lua_pushnumber(L, output.normal.x);
    lua_pushnumber(L, output.normal.y);
    lua_pushnumber(L, output.fraction);

    return 3;
}

int Shape::ComputeAABB(lua_State* L) const
{
    this->ThrowIfShapeNotValid();

    float x     = Physics::ScaleDown(luaL_checknumber(L, 1));
    float y     = Physics::ScaleDown(luaL_checknumber(L, 2));
    float angle = luaL_checknumber(L, 3);
    int index   = luaL_optinteger(L, 4, 1) - 1;

    b2Transform transform(b2Vec2(x, y), b2Rot(angle));
    b2AABB aabb {};

    this->shape->ComputeAABB(&aabb, transform, index);
    aabb = Physics::ScaleUp(aabb);

    lua_pushnumber(L, aabb.lowerBound.x);
    lua_pushnumber(L, aabb.lowerBound.y);
    lua_pushnumber(L, aabb.upperBound.x);
    lua_pushnumber(L, aabb.upperBound.y);

    return 4;
}

int Shape::ComputeMass(lua_State* L) const
{
    this->ThrowIfShapeNotValid();

    float density = luaL_checknumber(L, 1);

    b2MassData massData {};
    this->shape->ComputeMass(&massData, density);

    b2Vec2 center = Physics::ScaleUp(massData.center);

    lua_pushnumber(L, center.x);
    lua_pushnumber(L, center.y);
    lua_pushnumber(L, massData.mass);
    lua_pushnumber(L, Physics::ScaleUp(Physics::ScaleUp(massData.I)));

    return 4;
}

int Shape::GetBoundingBox(lua_State* L) const
{
    this->ThrowIfFixtureNotValid();

    int index = luaL_optinteger(L, 1, 1) - 1;

    b2AABB aabb {};
    luax::CatchException(L, [&]() { aabb = this->fixture->GetAABB(index); });

    aabb = Physics::ScaleUp(aabb);

    lua_pushnumber(L, aabb.lowerBound.x);
    lua_pushnumber(L, aabb.lowerBound.y);
    lua_pushnumber(L, aabb.upperBound.x);
    lua_pushnumber(L, aabb.upperBound.y);

    return 4;
}

int Shape::GetMassData(lua_State* L) const
{
    this->ThrowIfFixtureNotValid();

    b2MassData massData {};
    this->fixture->GetMassData(&massData);

    b2Vec2 center = Physics::ScaleUp(massData.center);

    lua_pushnumber(L, center.x);
    lua_pushnumber(L, center.y);
    lua_pushnumber(L, massData.mass);
    lua_pushnumber(L, massData.I);

    return 4;
}
