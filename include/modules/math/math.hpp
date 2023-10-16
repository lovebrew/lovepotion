#pragma once

#include <common/math.hpp>
#include <common/module.hpp>
#include <common/vector.hpp>

#include <objects/beziercurve/beziercurve.hpp>
#include <objects/randomgenerator/randomgenerator.hpp>

#include <noise1234.h>
#include <simplexnoise1234.h>

#include <vector>

namespace love
{
    class Transform;

    class Math : public Module
    {
      public:
        struct Triangle
        {
            Triangle(const Vector2& x, const Vector2& y, const Vector2& z) : a(x), b(y), c(z)
            {}

            Vector2 a, b, c;
        };

        static std::vector<Triangle> Triangulate(const std::vector<Vector2>& polygon);

        static bool IsConvex(const std::vector<Vector2>& polygon);

        static float GammaToLinear(float color);

        static float LinearToGamma(float color);

        static inline double SimplexNoise1(double x)
        {
            return SimplexNoise1234::noise(x) * 0.5 + 0.5;
        }

        static inline double SimplexNoise2(double x, double y)
        {
            return SimplexNoise1234::noise(x, y) * 0.5 + 0.5;
        }

        static inline double SimplexNoise3(double x, double y, double z)
        {
            return SimplexNoise1234::noise(x, y, z) * 0.5 + 0.5;
        }

        static inline double SimplexNoise4(double x, double y, double z, double w)
        {
            return SimplexNoise1234::noise(x, y, z, w) * 0.5 + 0.5;
        }

        static inline double PerlinNoise1(double x)
        {
            return Noise1234::noise(x) * 0.5 + 0.5;
        }

        static inline double PerlinNoise2(double x, double y)
        {
            return Noise1234::noise(x, y) * 0.5 + 0.5;
        }

        static inline double PerlinNoise3(double x, double y, double z)
        {
            return Noise1234::noise(x, y, z) * 0.5 + 0.5;
        }

        static inline double PerlinNoise4(double x, double y, double z, double w)
        {
            return Noise1234::noise(x, y, z, w) * 0.5 + 0.5;
        }

        Math();

        virtual ~Math()
        {}

        RandomGenerator* GetRandomGenerator()
        {
            return &this->randomGenerator;
        }

        RandomGenerator* NewRandomGenerator() const;

        BezierCurve* NewBezierCurve(const std::vector<Vector2>& points) const;

        Transform* NewTransform() const;

        Transform* NewTransform(float x, float y, float a, float sx, float sy, float ox, float oy,
                                float kx, float ky) const;

        virtual ModuleType GetModuleType() const
        {
            return M_MATH;
        }

        virtual const char* GetName() const
        {
            return "love.math";
        }

      private:
        RandomGenerator randomGenerator;
    };
} // namespace love
