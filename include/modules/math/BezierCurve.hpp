#pragma once

#include "common/Object.hpp"
#include "common/Vector.hpp"

#include <vector>

namespace love
{
    class BezierCurve : public Object
    {
      public:
        static Type type;

        BezierCurve(const std::vector<Vector2>& controlPoints);

        size_t getDegree() const
        {
            return this->controlPoints.size() - 1;
        }

        BezierCurve getDerivative() const;

        const Vector2& getControlPoint(int i) const;

        void setControlPoint(int i, const Vector2& point);

        void insertControlPoint(const Vector2& point, int pos = -1);

        void removeControlPoint(int i);

        size_t getControlPointCount() const
        {
            return this->controlPoints.size();
        }

        void translate(const Vector2& offset);

        void rotate(double phi, const Vector2& center);

        void scale(double scale, const Vector2& center);

        Vector2 evaluate(double time) const;

        BezierCurve* getSegment(double start, double end) const;

        std::vector<Vector2> render(int accuracy = 4) const;

        std::vector<Vector2> renderSegment(double start, double end, int accuracy = 4) const;

      private:
        std::vector<Vector2> controlPoints;
    };
} // namespace love
