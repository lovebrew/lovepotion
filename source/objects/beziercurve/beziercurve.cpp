#include <objects/beziercurve/beziercurve.hpp>

#include <common/exception.hpp>

#include <algorithm>

using namespace love;

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
    points.resize(left.size() + right.size() - 1);

    for (size_t i = 0; i < left.size(); ++i)
        points[i] = left[i];

    for (size_t i = 1; i < right.size(); ++i)
        points[i - 1 + left.size()] = right[right.size() - i - 1];
}

Type BezierCurve::type("BezierCurve", &Object::type);

BezierCurve::BezierCurve(const std::vector<Vector2>& points) : controlPoints(points)
{}

BezierCurve BezierCurve::GetDerivative() const
{
    if (this->GetDegree() < 1)
        throw love::Exception("Cannot derive a curve of degree less than 1.");

    std::vector<Vector2> fordwardDifferences(this->controlPoints.size() - 1);
    float degree = this->GetDegree();

    for (size_t index = 0; index < fordwardDifferences.size(); ++index)
        fordwardDifferences[index] =
            (this->controlPoints[index + 1] - this->controlPoints[index]) * degree;

    return BezierCurve(fordwardDifferences);
}

const Vector2& BezierCurve::GetControlPoint(int index) const
{
    if (this->controlPoints.size() == 0)
        throw love::Exception("Curve contains no control points.");

    while (index < 0)
        index += this->controlPoints.size();

    while ((size_t)index >= this->controlPoints.size())
        index -= this->controlPoints.size();

    return this->controlPoints[index];
}

void BezierCurve::SetControlPoint(int index, const Vector2& point)
{
    if (this->controlPoints.size() == 0)
        throw love::Exception("Curve contains no control points.");

    while (index < 0)
        index += this->controlPoints.size();

    while ((size_t)index >= this->controlPoints.size())
        index -= this->controlPoints.size();

    this->controlPoints[index] = point;
}

void BezierCurve::InsertControlPoint(const Vector2& point, int index)
{
    if (this->controlPoints.size() == 0)
        index = 0;

    while (index < 0)
        index += this->controlPoints.size();

    while ((size_t)index > this->controlPoints.size())
        index -= this->controlPoints.size();

    this->controlPoints.insert(this->controlPoints.begin() + index, point);
}

void BezierCurve::RemoveControlPoint(int index)
{
    if (this->controlPoints.size() == 0)
        throw love::Exception("Curve contains no control points.");

    while (index < 0)
        index += this->controlPoints.size();

    while ((size_t)index >= this->controlPoints.size())
        index -= this->controlPoints.size();

    this->controlPoints.erase(this->controlPoints.begin() + index);
}

void BezierCurve::Translate(const Vector2& translation)
{
    for (size_t index = 0; index < this->controlPoints.size(); ++index)
        this->controlPoints[index] += translation;
}

void BezierCurve::Rotate(double phi, const Vector2& center)
{
    float cosine = std::cos(phi);
    float sine   = std::sin(phi);

    for (size_t index = 0; index < this->controlPoints.size(); ++index)
    {
        Vector2 point = this->controlPoints[index] - center;

        this->controlPoints[index].x = cosine * point.x - sine * point.y + center.x;
        this->controlPoints[index].y = sine * point.x + cosine * point.y + center.y;
    }
}

void BezierCurve::Scale(double scale, const Vector2& center)
{
    for (size_t index = 0; index < this->controlPoints.size(); ++index)
        this->controlPoints[index] = (this->controlPoints[index] - center) * scale + center;
}

Vector2 BezierCurve::Evaluate(double time) const
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

BezierCurve* BezierCurve::GetSegment(double start, double end) const
{
    if (start < 0 || end > 1)
        throw love::Exception("Invalid segment parameters: must be between 0 and 1.");

    if (end <= start)
        throw love::Exception("Invalid segment parameters: start must be smaller than end");

    std::vector<Vector2> points(this->controlPoints);
    std::vector<Vector2> left, right;

    left.reserve(points.size());
    right.reserve(points.size());

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

std::vector<Vector2> BezierCurve::Render(int accuracy) const
{
    if (this->controlPoints.size() < 2)
        throw love::Exception("Invalid Bezier curve: Not enough control points.");

    std::vector<Vector2> vertices(this->controlPoints);
    subdivide(vertices, accuracy);

    return vertices;
}

std::vector<Vector2> BezierCurve::RenderSegment(double start, double end, int accuracy) const
{
    if (this->controlPoints.size() < 2)
        throw love::Exception("Invalid Bezier curve: Not enough control points.");

    std::vector<Vector2> vertices(this->controlPoints);
    subdivide(vertices, accuracy);

    if (start == end)
        vertices.clear();
    else if (start < end)
    {
        size_t startIndex = start * vertices.size();
        size_t endIndex   = end * vertices.size() + 0.5;

        return std::vector<Vector2>(vertices.begin() + startIndex, vertices.begin() + endIndex);
    }
    else if (end > start)
    {
        size_t startIndex = start * vertices.size() + 0.5;
        size_t endIndex   = end * vertices.size();

        return std::vector<Vector2>(vertices.begin() + startIndex, vertices.begin() + endIndex);
    }

    return vertices;
}
