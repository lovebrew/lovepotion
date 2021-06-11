#include "edgeshape/edgeshape.h"

#include "body/body.h"
#include "world/world.h"

#include "modules/physics/physics.h"

using namespace love;

love::Type EdgeShape::type("EdgeShape", &Shape::type);

EdgeShape::EdgeShape(b2EdgeShape* shape, bool own) : Shape(shape, own)
{}

EdgeShape::~EdgeShape()
{}

void EdgeShape::SetNextVertex(float x, float y)
{
    b2EdgeShape* edgeShape = (b2EdgeShape*)this->shape;
    b2Vec2 vec(x, y);

    edgeShape->m_vertex3 = Physics::ScaleDown(vec);
}

b2Vec2 EdgeShape::GetNextVertex() const
{
    b2EdgeShape* edgeShape = (b2EdgeShape*)this->shape;

    return Physics::ScaleUp(edgeShape->m_vertex3);
}

void EdgeShape::SetPreviousVertex(float x, float y)
{
    b2EdgeShape* edgeShape = (b2EdgeShape*)this->shape;
    b2Vec2 vec(x, y);

    edgeShape->m_vertex0 = Physics::ScaleDown(vec);
}

b2Vec2 EdgeShape::GetPreviousVertex() const
{
    b2EdgeShape* edgeShape = (b2EdgeShape*)this->shape;

    return Physics::ScaleUp(edgeShape->m_vertex0);
}

int EdgeShape::GetPoints(lua_State* L)
{
    b2EdgeShape* edgeShape = (b2EdgeShape*)this->shape;

    b2Vec2 vec1 = Physics::ScaleUp(edgeShape->m_vertex1);
    b2Vec2 vec2 = Physics::ScaleUp(edgeShape->m_vertex2);

    lua_pushnumber(L, vec1.x);
    lua_pushnumber(L, vec1.y);

    lua_pushnumber(L, vec2.x);
    lua_pushnumber(L, vec2.y);

    return 4;
}
