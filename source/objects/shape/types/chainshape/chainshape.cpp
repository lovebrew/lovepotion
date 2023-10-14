#include <objects/shape/types/chainshape/chainshape.hpp>

#include <modules/physics/physics.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

using namespace love;

Type ChainShape::type("ChainShape", &Shape::type);

ChainShape::ChainShape(Body* body, const b2ChainShape& chainShape) : Shape(body, chainShape)
{}

ChainShape::~ChainShape()
{}

void ChainShape::SetNextVertex(float x, float y)
{
    this->ThrowIfShapeNotValid();

    b2Vec2 vertex(x, y);

    auto* chainShape         = (b2ChainShape*)this->shape;
    chainShape->m_nextVertex = Physics::ScaleDown(vertex);
}

void ChainShape::SetPreviousVertex(float x, float y)
{
    this->ThrowIfShapeNotValid();

    b2Vec2 vertex(x, y);

    auto* chainShape         = (b2ChainShape*)this->shape;
    chainShape->m_prevVertex = Physics::ScaleDown(vertex);
}

b2Vec2 ChainShape::GetNextVertex() const
{
    this->ThrowIfShapeNotValid();

    auto* chainShape = (b2ChainShape*)this->shape;
    return Physics::ScaleUp(chainShape->m_nextVertex);
}

b2Vec2 ChainShape::GetPreviousVertex() const
{
    this->ThrowIfShapeNotValid();

    auto* chainShape = (b2ChainShape*)this->shape;
    return Physics::ScaleUp(chainShape->m_prevVertex);
}

EdgeShape* ChainShape::GetChildEdge(int index) const
{
    this->ThrowIfShapeNotValid();

    auto* chainShape = (b2ChainShape*)this->shape;

    b2EdgeShape edgeShape {};
    chainShape->GetChildEdge(&edgeShape, index);

    return new EdgeShape(nullptr, edgeShape);
}

int ChainShape::GetVertexCount() const
{
    this->ThrowIfShapeNotValid();

    auto* chainShape = (b2ChainShape*)this->shape;
    return chainShape->m_count;
}

b2Vec2 ChainShape::GetPoint(int index) const
{
    this->ThrowIfShapeNotValid();

    auto* chainShape = (b2ChainShape*)this->shape;

    if (index < 0 || index >= chainShape->m_count)
        throw love::Exception("Physics error: index out of bounds.");

    const auto& vertex = chainShape->m_vertices[index];
    return Physics::ScaleUp(vertex);
}

const b2Vec2* ChainShape::GetPoints() const
{
    this->ThrowIfShapeNotValid();

    auto* chainShape = (b2ChainShape*)this->shape;
    return chainShape->m_vertices;
}
