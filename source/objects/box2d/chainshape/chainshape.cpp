#include "chainshape/chainshape.h"

#include "body/body.h"
#include "world/world.h"

#include "modules/physics/physics.h"

using namespace love;

love::Type ChainShape::type("ChainShape", &Shape::type);

ChainShape::ChainShape(b2ChainShape* chainShape, bool own) : Shape(chainShape, own)
{}

ChainShape::~ChainShape()
{}

void ChainShape::SetNextVertex(float x, float y)
{
    b2ChainShape* chainShape = (b2ChainShape*)this->shape;
    b2Vec2 vec(x, y);

    chainShape->m_nextVertex = Physics::ScaleDown(vec);
}

b2Vec2 ChainShape::GetNextVertex() const
{
    b2ChainShape* chainShape = (b2ChainShape*)this->shape;

    return Physics::ScaleUp(chainShape->m_nextVertex);
}

void ChainShape::SetPreviousVertex(float x, float y)
{
    b2ChainShape* chainShape = (b2ChainShape*)this->shape;
    b2Vec2 vec(x, y);

    chainShape->m_prevVertex = Physics::ScaleDown(vec);
}

b2Vec2 ChainShape::GetPreviousVertex() const
{
    b2ChainShape* chainShape = (b2ChainShape*)this->shape;

    return Physics::ScaleUp(chainShape->m_prevVertex);
}

EdgeShape* ChainShape::GetChildEdge(int index) const
{
    b2ChainShape* chainShape = (b2ChainShape*)this->shape;
    b2EdgeShape* edgeShape   = new b2EdgeShape();

    try
    {
        chainShape->GetChildEdge(edgeShape, index);
    }
    catch (love::Exception&)
    {
        delete edgeShape;
        throw;
    }

    return new EdgeShape(edgeShape, true);
}

int ChainShape::GetVertexCount() const
{
    b2ChainShape* chainShape = (b2ChainShape*)this->shape;

    return chainShape->m_count;
}

b2Vec2 ChainShape::GetPoint(int index) const
{
    b2ChainShape* chainShape = (b2ChainShape*)this->shape;

    if (index < 0 || index >= chainShape->m_count)
        throw love::Exception("Physics error: index out of bounds.");

    const b2Vec2& vec = chainShape->m_vertices[index];

    return Physics::ScaleUp(vec);
}

const b2Vec2* ChainShape::GetPoints() const
{
    b2ChainShape* chainShape = (b2ChainShape*)this->shape;

    return chainShape->m_vertices;
}
