#include "objects/box2d/shape/shape.h"
#include "common/bidirectionalmap.h"
#include "modules/physics/physics.h"

using namespace love;

love::Type Shape::type("Shape", &Object::type);

Shape::Shape() : shape(nullptr), own(false)
{}

Shape::Shape(b2Shape* shape, bool own) : shape(shape), own(own)
{}

Shape::~Shape()
{
    if (this->shape && this->own)
        delete this->shape;

    this->shape = nullptr;
}

Shape::Type Shape::GetType() const
{
    switch (shape->GetType())
    {
        case b2Shape::e_circle:
            return SHAPE_CIRCLE;
        case b2Shape::e_polygon:
            return SHAPE_POLYGON;
        case b2Shape::e_edge:
            return SHAPE_EDGE;
        case b2Shape::e_chain:
            return SHAPE_CHAIN;
        default:
            return SHAPE_INVALID;
    }
}

float Shape::GetRadius() const
{
    return Physics::ScaleUp(this->shape->m_radius);
}

int Shape::GetChildCount() const
{
    return this->shape->GetChildCount();
}

bool Shape::TestPoint(float x, float y, float radius, float px, float py) const
{
    b2Vec2 point(px, py);
    b2Transform transform(Physics::ScaleDown(b2Vec2(x, y)), b2Rot(radius));

    return this->shape->TestPoint(transform, Physics::ScaleDown(point));
}

int Shape::RayCast(lua_State* L) const
{
    float pointA = Physics::ScaleDown(luaL_checknumber(L, 1));
    float pointB = Physics::ScaleDown(luaL_checknumber(L, 2));

    float pointC = Physics::ScaleDown(luaL_checknumber(L, 3));
    float pointD = Physics::ScaleDown(luaL_checknumber(L, 4));

    float maxFraction = luaL_checknumber(L, 5);

    float x = Physics::ScaleDown(luaL_checknumber(L, 6));
    float y = Physics::ScaleDown(luaL_checknumber(L, 7));
    float r = luaL_checknumber(L, 8);

    int childIndex = luaL_optinteger(L, 9, 1) - 1; // Convert from 1-based index

    b2RayCastInput input;
    input.p1.Set(pointA, pointB);
    input.p2.Set(pointC, pointD);
    input.maxFraction = maxFraction;

    b2Transform transform(b2Vec2(x, y), b2Rot(r));

    b2RayCastOutput output;

    if (!this->shape->RayCast(&output, input, transform, childIndex))
        return 0;

    lua_pushnumber(L, output.normal.x);
    lua_pushnumber(L, output.normal.y);
    lua_pushnumber(L, output.fraction);

    return 3;
}

int Shape::ComputeAABB(lua_State* L) const
{
    float x        = Physics::ScaleDown(luaL_checknumber(L, 1));
    float y        = Physics::ScaleDown(luaL_checknumber(L, 2));
    float r        = luaL_checknumber(L, 3);
    int childIndex = luaL_optinteger(L, 4, 1) - 1; // Convert from 1-based index

    b2Transform transform(b2Vec2(x, y), b2Rot(r));

    b2AABB box;
    this->shape->ComputeAABB(&box, transform, childIndex);
    box = Physics::ScaleUp(box);

    lua_pushnumber(L, box.lowerBound.x);
    lua_pushnumber(L, box.lowerBound.y);
    lua_pushnumber(L, box.upperBound.x);
    lua_pushnumber(L, box.upperBound.y);

    return 4;
}

int Shape::ComputeMass(lua_State* L) const
{
    float density = (float)luaL_checknumber(L, 1);

    b2MassData data;
    this->shape->ComputeMass(&data, density);

    b2Vec2 center = Physics::ScaleUp(data.center);

    lua_pushnumber(L, center.x);
    lua_pushnumber(L, center.y);
    lua_pushnumber(L, data.mass);
    lua_pushnumber(L, Physics::ScaleUp(Physics::ScaleUp(data.I)));

    return 4;
}

// clang-format off
constexpr auto types = BidirectionalMap<>::Create(
    "circle",  Shape::SHAPE_CIRCLE,
    "polygon", Shape::SHAPE_POLYGON,
    "edge",    Shape::SHAPE_EDGE,
    "chain",   Shape::SHAPE_CHAIN
);
// clang-format on

bool Shape::GetConstant(const char* in, Shape::Type& out)
{
    return types.Find(in, out);
}

bool Shape::GetConstant(Shape::Type in, const char*& out)
{
    return types.ReverseFind(in, out);
}
