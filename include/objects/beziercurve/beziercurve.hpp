#pragma once

#include <common/object.hpp>
#include <common/vector.hpp>

#include <vector>

namespace love
{
    class BezierCurve : public Object
    {
      public:
        static Type type;

        BezierCurve(const std::vector<Vector2>& controlPoints);

        size_t GetDegree() const
        {
            return this->controlPoints.size() - 1;
        }

        BezierCurve GetDerivative() const;

        const Vector2& GetControlPoint(int index) const;

        void SetControlPoint(int index, const Vector2& point);

        void InsertControlPoint(const Vector2& point, int position = -1);

        void RemoveControlPoint(int index);

        size_t GetControlPointCount() const
        {
            return this->controlPoints.size();
        }

        void Translate(const Vector2& translation);

        void Rotate(double phi, const Vector2& center);

        void Scale(double scale, const Vector2& center);

        Vector2 Evaluate(double time) const;

        BezierCurve* GetSegment(double start, double end) const;

        std::vector<Vector2> Render(int accuracy = 4) const;

        std::vector<Vector2> RenderSegment(double start, double end, int accuracy = 4) const;

      private:
        std::vector<Vector2> controlPoints;
    };
} // namespace love
