#include <objects/shape/types/edgeshape/edgeshape.hpp>

#include <modules/physics/physics.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

using namespace love;

Type EdgeShape::type("EdgeShape", &Shape::type);

EdgeShape::EdgeShape(Body* body, const b2EdgeShape& edgeShape) : Shape(body, edgeShape)
{}

EdgeShape::~EdgeShape()
{}

void EdgeShape::SetNextVertex(float x, float y)
{
    this->ThrowIfShapeNotValid();

    auto* edgeShape = (b2EdgeShape*)this->shape;
    b2Vec2 vertex(x, y);

    edgeShape->m_vertex3 = Physics::ScaleDown(vertex);
}

b2Vec2 EdgeShape::GetNextVertex() const
{
    this->ThrowIfShapeNotValid();

    auto* edgeShape = (b2EdgeShape*)this->shape;
    return Physics::ScaleUp(edgeShape->m_vertex3);
}

void EdgeShape::SetPreviousVertex(float x, float y)
{
    this->ThrowIfShapeNotValid();

    auto* edgeShape = (b2EdgeShape*)this->shape;
    b2Vec2 vertex(x, y);

    edgeShape->m_vertex0 = Physics::ScaleDown(vertex);
}

b2Vec2 EdgeShape::GetPreviousVertex() const
{
    this->ThrowIfShapeNotValid();

    auto* edgeShape = (b2EdgeShape*)this->shape;
    return Physics::ScaleUp(edgeShape->m_vertex0);
}

int EdgeShape::GetPoints(lua_State* L)
{
    this->ThrowIfShapeNotValid();

    auto* edgeShape = (b2EdgeShape*)this->shape;

    b2Vec2 first  = Physics::ScaleUp(edgeShape->m_vertex1);
    b2Vec2 second = Physics::ScaleUp(edgeShape->m_vertex2);

    lua_pushnumber(L, first.x);
    lua_pushnumber(L, first.y);
    lua_pushnumber(L, second.x);
    lua_pushnumber(L, second.y);

    return 4;
}
