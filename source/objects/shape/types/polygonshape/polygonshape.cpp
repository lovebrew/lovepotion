#include <objects/shape/types/polygonshape/polygonshape.hpp>

#include <modules/physics/physics.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

using namespace love;

Type PolygonShape::type("PolygonShape", &Shape::type);

PolygonShape::PolygonShape(Body* body, const b2PolygonShape& polygonShape) :
    Shape(body, polygonShape)
{}

PolygonShape::~PolygonShape()
{}

int PolygonShape::GetPoints(lua_State* L)
{
    this->ThrowIfShapeNotValid();
    luax::AssertArgCount<0>(L);

    auto* polygonShape = (b2PolygonShape*)this->shape;
    int count          = polygonShape->m_count;

    for (int index = 0; index < count; index++)
    {
        auto point = Physics::ScaleUp(polygonShape->m_vertices[index]);

        lua_pushnumber(L, point.x);
        lua_pushnumber(L, point.y);
    }

    return count * 2;
}

bool PolygonShape::Validate() const
{
    this->ThrowIfShapeNotValid();

    auto* polygonShape = (b2PolygonShape*)this->shape;
    return polygonShape->Validate();
}
