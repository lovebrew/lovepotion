#pragma once

#include "shape/shape.h"
#include <vector>

namespace love
{
    class CircleShape : public Shape
    {
      public:
        static love::Type type;

        CircleShape(b2CircleShape* circleShape, bool own = true);

        virtual ~CircleShape();

        float GetRadius() const;

        void SetRadius(float radius);

        void GetPoint(float& xOut, float& yOut) const;

        void SetPoint(float x, float y);
    };
} // namespace love
