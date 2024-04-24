#include "common/Exception.hpp"

#include "modules/math/BezierCurve.hpp"

#include <algorithm>
#include <cmath>

namespace love
{
    static void subdivide(std::vector<Vector2>& points, int k)
    {
        if (k <= 0)
            return;

        // subdivision using de casteljau - subdivided control polygons are
        // on the 'edges' of the computation scheme, e.g:
        //
        // ------LEFT------->
        // b00  b10  b20  b30
        // b01  b11  b21 .---
        // b02  b12 .---'
        // b03 .---'RIGHT
        // <--'
        //
        // the subdivided control polygon is:
        // b00, b10, b20, b30, b21, b12, b03
        std::vector<Vector2> left, right;

        left.reserve(points.size());
        right.reserve(points.size());

        for (size_t step = 1; step < points.size(); ++step)
        {
            left.push_back(points[0]);
            right.push_back(points[points.size() - step]);

            for (size_t i = 0; i < points.size() - step; ++i)
                points[i] = (points[i] + points[i + 1]) * .5;
        }

        left.push_back(points[0]);
        right.push_back(points[0]);

        // recurse
        subdivide(left, k - 1);
        subdivide(right, k - 1);

        // merge (right is in reversed order)
        // By this point the 'left' array has a point at the end that's collinear
        // with other points. It's still needed for the subdivide calls above but we
        // can get rid of it here.
        points.resize(left.size() + right.size() - 2);

        for (size_t i = 0; i < left.size() - 1; ++i)
            points[i] = left[i];

        for (size_t i = 1; i < right.size(); ++i)
            points[i - 1 + left.size() - 1] = right[right.size() - i - 1];
    }

    Type BezierCurve::type("BezierCurve", &Object::type);

    BezierCurve::BezierCurve(const std::vector<Vector2>& controlPoints) : controlPoints(controlPoints)
    {}

    BezierCurve BezierCurve::getDerivative() const
    {
        if (this->getDegree() < 1)
            throw love::Exception("Cannot derive a curve of degree < 1.");

        std::vector<Vector2> differences(this->controlPoints.size() - 1);
        const float degree = this->getDegree();

        for (size_t index = 0; index < differences.size(); ++index)
            differences[index] = (this->controlPoints[index + 1] - this->controlPoints[index]) * degree;

        return BezierCurve(differences);
    }

    const Vector2& BezierCurve::getControlPoint(int index) const
    {
        if (this->controlPoints.size() == 0)
            throw love::Exception("Curve contains no control points.");

        while (index < 0)
            index += this->controlPoints.size();

        while ((size_t)index >= this->controlPoints.size())
            index -= this->controlPoints.size();

        return this->controlPoints[index];
    }

    void BezierCurve::setControlPoint(int index, const Vector2& point)
    {
        if (this->controlPoints.size() == 0)
            throw love::Exception("Curve contains no control points.");

        while (index < 0)
            index += this->controlPoints.size();

        while ((size_t)index >= this->controlPoints.size())
            index -= this->controlPoints.size();

        this->controlPoints[index] = point;
    }

    void BezierCurve::insertControlPoint(const Vector2& point, int index)
    {
        if (this->controlPoints.size() == 0)
            index = 0;

        while (index < 0)
            index += this->controlPoints.size();

        while ((size_t)index > this->controlPoints.size())
            index -= this->controlPoints.size();

        this->controlPoints.insert(this->controlPoints.begin() + index, point);
    }

    void BezierCurve::removeControlPoint(int index)
    {
        if (this->controlPoints.size() == 0)
            throw love::Exception("Curve contains no control points.");

        while (index < 0)
            index += this->controlPoints.size();

        while ((size_t)index >= this->controlPoints.size())
            index -= this->controlPoints.size();

        this->controlPoints.erase(this->controlPoints.begin() + index);
    }

    void BezierCurve::translate(const Vector2& offset)
    {
        for (size_t index = 0; index < this->controlPoints.size(); ++index)
            this->controlPoints[index] += offset;
    }

    void BezierCurve::rotate(double phi, const Vector2& center)
    {
        const float c = std::cos(phi);
        const float s = std::sin(phi);

        for (size_t index = 0; index < this->controlPoints.size(); ++index)
        {
            Vector2 point = this->controlPoints[index] - center;

            this->controlPoints[index].x = c * point.x - s * point.y + center.x;
            this->controlPoints[index].y = s * point.x + c * point.y + center.y;
        }
    }

    void BezierCurve::scale(double scale, const Vector2& center)
    {
        for (size_t index = 0; index < this->controlPoints.size(); ++index)
            this->controlPoints[index] = (this->controlPoints[index] - center) * scale + center;
    }

    Vector2 BezierCurve::evaluate(double time) const
    {
        if (time < 0 || time > 1)
            throw love::Exception("Invalid evaluation parameter: must be between 0 and 1");

        if (this->controlPoints.size() < 2)
            throw love::Exception("Invalid Bezier curve: Not enough control points.");

        std::vector<Vector2> points(this->controlPoints);

        for (size_t step = 1; step < this->controlPoints.size(); ++step)
        {
            for (size_t index = 0; index < this->controlPoints.size() - step; ++index)
                points[index] = points[index] * (1 - time) + points[index + 1] * time;
        }

        return points[0];
    }

    BezierCurve* BezierCurve::getSegment(double start, double end) const
    {
        if (start < 0 || end > 1)
            throw love::Exception("Invalid segment parameters: must be between 0 and 1");

        if (end <= start)
            throw love::Exception("Invalid segment parameters: end must be greater than start");

        std::vector<Vector2> points(this->controlPoints);
        std::vector<Vector2> left {}, right {};

        for (size_t step = 1; step < points.size(); ++step)
        {
            left.push_back(points[0]);
            for (size_t index = 0; index < points.size() - step; ++index)
                points[index] += (points[index + 1] - points[index]) * end;
        }
        left.push_back(points[0]);

        double subdivision = start / end;
        for (size_t step = 1; step < left.size(); ++step)
        {
            right.push_back(left[left.size() - step]);
            for (size_t index = 0; index < left.size() - step; ++index)
                left[index] += (left[index + 1] - left[index]) * subdivision;
        }
        right.push_back(left[0]);

        std::reverse(right.begin(), right.end());

        return new BezierCurve(right);
    }

    std::vector<Vector2> BezierCurve::render(int accuracy) const
    {
        if (this->controlPoints.size() < 2)
            throw love::Exception("Invalid Bezier curve: Not enough control points.");

        std::vector<Vector2> points(this->controlPoints);
        subdivide(points, accuracy);

        return points;
    }

    std::vector<Vector2> BezierCurve::renderSegment(double start, double end, int accuracy) const
    {
        if (this->controlPoints.size() < 2)
            throw love::Exception("Invalid Bezier curve: Not enough control points.");

        std::vector<Vector2> points(this->controlPoints);
        subdivide(points, accuracy);

        if (start == end)
            points.clear();
        else if (start < end)
        {
            size_t startIndex = (size_t)(start * points.size());
            size_t endIndex   = (size_t)(end * points.size() + 0.5);

            return std::vector<Vector2>(points.begin() + startIndex, points.begin() + endIndex);
        }
        else if (end > start)
        {
            size_t startIndex = (size_t)(end * points.size() + 0.5);
            size_t endIndex   = (size_t)(start * points.size());

            return std::vector<Vector2>(points.begin() + startIndex, points.begin() + endIndex);
        }

        return points;
    }
} // namespace love
