#pragma once

#include "common/vector.h"

// Noise
#include "noise1234/noise1234.h"
#include "noise1234/simplexnoise1234.h"

#include "common/mmath.h"

#include "objects/random/randomgenerator.h"

#include <list>
#include <vector>

#include "common/exception.h"

namespace love
{
    class Math : public Module
    {
        public:
            struct Triangle
            {
                Triangle(const Vector2 & x, const Vector2 & y, const Vector2 & z) : a(x),
                                                                                    b(y),
                                                                                    c(z)
                {}

                Vector2 a, b, c;
            };

            Math();
            ~Math() {};

            ModuleType GetModuleType() const { return M_MATH; }

            const char * GetName() const override { return "love.math"; }

            /* RandomGenerator */

            RandomGenerator * GetRandomGenerator() {
                return &this->rng;
            }

            RandomGenerator * NewRandomGenerator();

            /* LÖVE Functions */

            float GammaToLinear(float color);

            float LinearToGamma(float color);

            std::vector<Triangle> Triangulate(const std::vector<Vector2> & polygon);

            bool IsConvex(const std::vector<Vector2> & polygon);

        private:
            RandomGenerator rng;

            /* Helper Functions */

            inline bool IsCounterClockwise(const Vector2 & a, const Vector2 & b, const Vector2 & c) {
                return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)) >= 0;
            }

            inline bool OnSameSide(const Vector2 & a, const Vector2 & b, const Vector2 & c, const Vector2 & d) {
                float px = d.x - c.x, py = d.y - c.y;

                // return det(p, a-c) * det(p, b-c) >= 0
                float l = px * (a.y - c.y) - py * (a.x - c.x);
                float m = px * (b.y - c.y) - py * (b.x - c.x);

                return l * m >= 0;
            }

            inline bool PointInTriangle(const Vector2 & p, const Vector2 & a, const Vector2 & b, const Vector2 & c) {
                return OnSameSide(p, a, b,c) && OnSameSide(p, b, a, c) && OnSameSide(p, c, a, b);
            }

            inline bool AnyPointInTriangle(const std::list<const Vector2 *> & vertices, const Vector2 & a, const Vector2 & b, const Vector2 & c) {
                for (const Vector2 * p : vertices)
                {
                    if ((p != & a) && (p != & b) && (p != & c) && PointInTriangle(*p, a, b, c)) // oh god...
                        return true;
                }

                return false;
            }

            inline bool IsEar(const Vector2 & a, const Vector2 & b, const Vector2 & c, const std::list<const Vector2 *> & vertices) {
                return IsCounterClockwise(a, b, c) && !AnyPointInTriangle(vertices, a, b, c);
            }

    };
}
