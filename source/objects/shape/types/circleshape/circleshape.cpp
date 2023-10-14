#include <objects/shape/types/circleshape/circleshape.hpp>

#include <modules/physics/physics.hpp>

#include <objects/body/body.hpp>
#include <objects/world/world.hpp>

using namespace love;

Type CircleShape::type("CircleShape", &Shape::type);

CircleShape::CircleShape(Body* body, const b2CircleShape& circleShape) : Shape(body, circleShape)
{}

CircleShape::~CircleShape()
{}

float CircleShape::GetRadius() const
{
    this->ThrowIfShapeNotValid();

    return Physics::ScaleUp(shape->m_radius);
}

void CircleShape::SetRadius(float radius)
{
    this->ThrowIfShapeNotValid();

    shape->m_radius = Physics::ScaleDown(radius);
}

void CircleShape::GetPoint(float& x, float& y) const
{
    this->ThrowIfShapeNotValid();

    auto* circleShape = (b2CircleShape*)this->shape;

    x = Physics::ScaleUp(circleShape->m_p.x);
    y = Physics::ScaleUp(circleShape->m_p.y);
}

void CircleShape::SetPoint(float x, float y)
{
    this->ThrowIfShapeNotValid();

    auto* circleShape = (b2CircleShape*)this->shape;
    circleShape->m_p  = Physics::ScaleDown(b2Vec2(x, y));
}
