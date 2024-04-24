#pragma once

#include "modules/math/RandomGenerator.hpp"

#include "common/Module.hpp"
#include "common/StrongRef.hpp"
#include "common/Vector.hpp"
#include "common/int.hpp"
#include "common/math.hpp"

#include "noise1234.h"
#include "simplexnoise1234.h"

#include <vector>

namespace love
{
    class BezierCurve;
    class Transform;

    struct Triangle
    {
        Triangle(const Vector2& x, const Vector2& y, const Vector2& z) : a(x), b(y), c(z)
        {}

        Vector2 a, b, c;
    };

    std::vector<Triangle> triangulate(const std::vector<love::Vector2>& polygon);

    bool isConvex(const std::vector<Vector2>& polygon);

    float gammaToLinear(float c);

    float linearToGamma(float c);

    static inline double simplexNoise1(double x)
    {
        return SimplexNoise1234::noise(x) * 0.5 + 0.5;
    }

    static inline double simplexNoise2(double x, double y)
    {
        return SimplexNoise1234::noise(x, y) * 0.5 + 0.5;
    }

    static inline double simplexNoise3(double x, double y, double z)
    {
        return SimplexNoise1234::noise(x, y, z) * 0.5 + 0.5;
    }

    static inline double simplexNoise4(double x, double y, double z, double w)
    {
        return SimplexNoise1234::noise(x, y, z, w) * 0.5 + 0.5;
    }

    static inline double perlinNoise1(double x)
    {
        return Noise1234::noise(x) * 0.5 + 0.5;
    }

    static inline double perlinNoise2(double x, double y)
    {
        return Noise1234::noise(x, y) * 0.5 + 0.5;
    }

    static inline double perlinNoise3(double x, double y, double z)
    {
        return Noise1234::noise(x, y, z) * 0.5 + 0.5;
    }

    static inline double perlinNoise4(double x, double y, double z, double w)
    {
        return Noise1234::noise(x, y, z, w) * 0.5 + 0.5;
    }

    class Math : public Module
    {
      public:
        Math();

        virtual ~Math();

        RandomGenerator* getRandomGenerator()
        {
            return this->random.get();
        }

        RandomGenerator* newRandomGenerator() const;

        BezierCurve* newBezierCurve(const std::vector<Vector2>& points) const;

        Transform* newTransform() const;

        Transform* newTransform(float x, float y, float angle, float sx, float sy, float ox, float oy,
                                float kx, float ky) const;

      private:
        StrongRef<RandomGenerator> random;
    };
} // namespace love
