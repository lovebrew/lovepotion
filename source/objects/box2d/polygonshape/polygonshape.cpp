#include "polygonshape/polygonshape.h"

#include "modules/physics/physics.h"

using namespace love;

love::Type PolygonShape::type("PolygonShape", &Shape::type);

PolygonShape::PolygonShape(b2PolygonShape* polygonShape, bool own) : Shape(polygonShape, own)
{}

PolygonShape::~PolygonShape()
{}

int PolygonShape::GetPoints(lua_State* L)
{
    Luax::AssertArgc<0>(L);

    b2PolygonShape* polygonShape = (b2PolygonShape*)this->shape;
    int count                    = polygonShape->m_count;

    for (int index = 0; index < count; index++)
    {
        b2Vec2 vec = Physics::ScaleUp(polygonShape->m_vertices[index]);

        lua_pushnumber(L, vec.x);
        lua_pushnumber(L, vec.y);
    }

    return count * 2;
}

bool PolygonShape::Validate() const
{
    b2PolygonShape* polygonShape = (b2PolygonShape*)this->shape;

    return polygonShape->Validate();
}
