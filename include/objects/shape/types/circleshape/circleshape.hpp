#pragma once

#include <objects/shape/shape.hpp>

#include <vector>

namespace love
{
    class CircleShape : public Shape
    {
      public:
        static love::Type type;

        CircleShape(Body* body, const b2CircleShape& circleShape);

        virtual ~CircleShape();

        float GetRadius() const;

        void SetRadius(float radius);

        void GetPoint(float& x, float& y) const;

        void SetPoint(float x, float y);
    };
} // namespace love
