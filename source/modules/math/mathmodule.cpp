#include "modules/math/mathmodule.h"

#include "common/vector.h"
#include "objects/transform/transform.h"

#include <cmath>
#include <list>

using namespace love;

Math::Math() : rng()
{
    RandomGenerator::Seed seed;
    seed.b64 = (uint64_t)time(nullptr);

    rng.SetSeed(seed);
}

RandomGenerator* Math::NewRandomGenerator()
{
    return new RandomGenerator();
}

Transform* Math::NewTransform()
{
    return new Transform();
}

Transform* Math::NewTransform(float x, float y, float a, float sx, float sy, float ox, float oy,
                              float kx, float ky)
{
    return new Transform(x, y, a, sx, sy, ox, oy, kx, ky);
}

std::vector<Math::Triangle> Math::Triangulate(const std::vector<Vector2>& polygon)
{
    if (polygon.size() < 3)
        throw love::Exception("Not a polygon");
    else if (polygon.size() == 3)
        return std::vector<Triangle>(1, Triangle(polygon[0], polygon[1], polygon[2]));

    // collect list of connections and record leftmost item to check if the polygon
    // has the expected winding
    std::vector<size_t> next_idx(polygon.size()), prev_idx(polygon.size());
    size_t idx_lm = 0;

    for (size_t i = 0; i < polygon.size(); ++i)
    {
        const love::Vector2& lm = polygon[idx_lm];
        const love::Vector2& p  = polygon[i];

        if (p.x < lm.x || (p.x == lm.x && p.y < lm.y))
            idx_lm = i;

        next_idx[i] = i + 1;
        prev_idx[i] = i - 1;
    }

    next_idx[next_idx.size() - 1] = 0;
    prev_idx[0]                   = prev_idx.size() - 1;

    // check if the polygon has the expected winding and reverse polygon if needed
    if (!this->IsCounterClockwise(polygon[prev_idx[idx_lm]], polygon[idx_lm],
                                  polygon[next_idx[idx_lm]]))
        next_idx.swap(prev_idx);

    // collect list of concave polygons
    std::list<const love::Vector2*> concave_vertices;

    for (size_t i = 0; i < polygon.size(); ++i)
    {
        if (!this->IsCounterClockwise(polygon[prev_idx[i]], polygon[i], polygon[next_idx[i]]))
            concave_vertices.push_back(&polygon[i]);
    }

    // triangulation according to kong
    std::vector<Triangle> triangles;
    size_t n_vertices = polygon.size();
    size_t current = 1, skipped = 0, next, prev;

    while (n_vertices > 3)
    {
        next             = next_idx[current];
        prev             = prev_idx[current];
        const Vector2 &a = polygon[prev], &b = polygon[current], &c = polygon[next];
        if (this->IsEar(a, b, c, concave_vertices))
        {
            triangles.push_back(Triangle(a, b, c));
            next_idx[prev] = next;
            prev_idx[next] = prev;
            concave_vertices.remove(&b);
            --n_vertices;
            skipped = 0;
        }
        else if (++skipped > n_vertices)
            throw love::Exception("Cannot triangulate polygon.");

        current = next;
    }

    next = next_idx[current];
    prev = prev_idx[current];

    triangles.push_back(Triangle(polygon[prev], polygon[current], polygon[next]));

    return triangles;
}

bool Math::IsConvex(const std::vector<Vector2>& polygon)
{
    if (polygon.size() < 3)
        return false;

    // a polygon is convex if all corners turn in the same direction
    // turning direction can be determined using the cross-product of
    // the forward difference vectors
    size_t i = polygon.size() - 2;
    size_t j = polygon.size() - 1;
    size_t k = 0;

    Vector2 p(polygon[j] - polygon[i]);
    Vector2 q(polygon[k] - polygon[j]);

    float winding = Vector2::cross(p, q);

    while (k + 1 < polygon.size())
    {
        i = j;
        j = k;
        k++;

        p = polygon[j] - polygon[i];
        q = polygon[k] - polygon[j];

        if (Vector2::cross(p, q) * winding < 0)
            return false;
    }

    return true;
}

float Math::GammaToLinear(float color)
{
    if (color <= 0.04045f)
        return color / 12.92f;
    else
        return powf((color + 0.055f) / 1.055f, 2.4f);
}

float Math::LinearToGamma(float color)
{
    if (color <= 0.0031308f)
        return color * 12.92f;
    else
        return 1.055f * powf(color, 1.0f / 2.4f) - 0.055f;
}
