#include "circleshape/circleshape.h"

#include "modules/physics/physics.h"

using namespace love;

love::Type CircleShape::type("CircleShape", &Shape::type);

CircleShape::CircleShape(b2CircleShape* circleShape, bool own) : Shape(circleShape, own)
{}

CircleShape::~CircleShape()
{}

float CircleShape::GetRadius() const
{
    return Physics::ScaleUp(this->shape->m_radius);
}

void CircleShape::SetRadius(float radius)
{
    this->shape->m_radius = Physics::ScaleDown(radius);
}

void CircleShape::GetPoint(float& xOut, float& yOut) const
{
    b2CircleShape* circleShape = (b2CircleShape*)this->shape;

    xOut = Physics::ScaleUp(circleShape->m_p.x);
    yOut = Physics::ScaleUp(circleShape->m_p.y);
}

void CircleShape::SetPoint(float x, float y)
{
    b2CircleShape* circleShape = (b2CircleShape*)this->shape;

    circleShape->m_p = Physics::ScaleDown(b2Vec2(x, y));
}
