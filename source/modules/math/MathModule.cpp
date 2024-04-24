#include "modules/math/MathModule.hpp"

#include "common/Map.hpp"
#include "common/Vector.hpp"
#include "common/int.hpp"

#include "modules/math/BezierCurve.hpp"
#include "modules/math/Transform.hpp"

#include <cmath>
#include <list>

#include <time.h>

namespace love
{
    inline bool isOrientedCCW(const Vector2& a, const Vector2& b, const Vector2& c)
    {
        return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)) >= 0;
    }

    inline bool onSameSide(const Vector2& a, const Vector2& b, const Vector2& c, const Vector2& d)
    {
        float px = d.x - c.x, py = d.y - c.y;

        float l = px * (a.y - c.y) - py * (a.x - c.x);
        float m = px * (b.y - c.y) - py * (b.x - c.x);

        return l * m >= 0;
    }

    inline bool pointInTriangle(const Vector2& p, const Vector2& a, const Vector2& b, const Vector2& c)
    {
        return onSameSide(p, a, b, c) && onSameSide(p, b, a, c) && onSameSide(p, c, a, b);
    }

    bool anyPointInTriangle(const std::list<const Vector2*>& vertices, const Vector2& a, const Vector2& b,
                            const Vector2& c)
    {
        for (const Vector2* p : vertices)
        {
            if ((p != &a) && (p != &b) && (p != &c) && pointInTriangle(*p, a, b, c)) // oh god...
                return true;
        }

        return false;
    }

    inline bool isEar(const Vector2& a, const Vector2& b, const Vector2& c,
                      const std::list<const Vector2*>& vertices)
    {
        return isOrientedCCW(a, b, c) && !anyPointInTriangle(vertices, a, b, c);
    }

    std::vector<Triangle> triangulate(const std::vector<love::Vector2>& polygon)
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
            const love::Vector2 &lm = polygon[idx_lm], &p = polygon[i];
            if (p.x < lm.x || (p.x == lm.x && p.y < lm.y))
                idx_lm = i;

            next_idx[i] = i + 1;
            prev_idx[i] = i - 1;
        }

        next_idx[next_idx.size() - 1] = 0;
        prev_idx[0]                   = prev_idx.size() - 1;

        // check if the polygon has the expected winding and reverse polygon if needed
        if (!isOrientedCCW(polygon[prev_idx[idx_lm]], polygon[idx_lm], polygon[next_idx[idx_lm]]))
            next_idx.swap(prev_idx);

        // collect list of concave polygons
        std::list<const love::Vector2*> concave_vertices;
        for (size_t i = 0; i < polygon.size(); ++i)
        {
            if (!isOrientedCCW(polygon[prev_idx[i]], polygon[i], polygon[next_idx[i]]))
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
            if (isEar(a, b, c, concave_vertices))
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

    bool isConvex(const std::vector<love::Vector2>& polygon)
    {
        if (polygon.size() < 3)
            return false;

        // a polygon is convex if all corners turn in the same direction
        // turning direction can be determined using the cross-product of
        // the forward difference vectors
        size_t i = polygon.size() - 2, j = polygon.size() - 1, k = 0;

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

    float gammaToLinear(float c)
    {
        if (c <= 0.04045f)
            return c / 12.92f;
        else
            return std::pow((c + 0.055f) / 1.055f, 2.4f);
    }

    float linearToGamma(float c)
    {
        if (c <= 0.0031308f)
            return c * 12.92f;
        else
            return 1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f;
    }

    Math::Math() : Module(M_MATH, "love.math")
    {
        RandomGenerator::Seed seed {};
        seed.b64 = (uint64_t)time(nullptr);

        this->random.set(new RandomGenerator(), Acquire::NO_RETAIN);
        this->random->setSeed(seed);
    }

    Math::~Math()
    {}

    RandomGenerator* Math::newRandomGenerator() const
    {
        return new RandomGenerator();
    }

    BezierCurve* Math::newBezierCurve(const std::vector<Vector2>& points) const
    {
        return new BezierCurve(points);
    }

    Transform* Math::newTransform() const
    {
        return new Transform();
    }

    Transform* Math::newTransform(float x, float y, float a, float sx, float sy, float ox, float oy, float kx,
                                  float ky) const
    {
        return new Transform(x, y, a, sx, sy, ox, oy, kx, ky);
    }
} // namespace love
