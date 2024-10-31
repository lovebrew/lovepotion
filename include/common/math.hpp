#pragma once

#include <climits> // for CHAR_BIT
#include <cstdio>
#include <cstdlib> // for rand() and RAND_MAX

#include <algorithm>

/* Definitions of useful mathematical constants
 * M_E        - e
 * M_LOG2E    - log2(e)
 * M_LOG10E   - log10(e)
 * M_LN2      - ln(2)
 * M_LN10     - ln(10)
 * M_PI       - pi
 * M_PI_2     - pi/2
 * M_PI_4     - pi/4
 * M_1_PI     - 1/pi
 * M_2_PI     - 2/pi
 * M_2_SQRTPI - 2/sqrt(pi)
 * M_SQRT2    - sqrt(2)
 * M_SQRT1_2  - 1/sqrt(2)
 */

#define LOVE_M_E        2.71828182845904523536
#define LOVE_M_LOG2E    1.44269504088896340736
#define LOVE_M_LOG10E   0.434294481903251827651
#define LOVE_M_LN2      0.693147180559945309417
#define LOVE_M_LN10     2.30258509299404568402
#define LOVE_M_PI       3.14159265358979323846
#define LOVE_M_PI_2     1.57079632679489661923
#define LOVE_M_PI_4     0.785398163397448309616
#define LOVE_M_1_PI     0.318309886183790671538
#define LOVE_M_2_PI     0.636619772367581343076
#define LOVE_M_2_SQRTPI 1.12837916709551257390
#define LOVE_M_SQRT2    1.41421356237309504880
#define LOVE_M_SQRT1_2  0.707106781186547524401
#define LOVE_M_TORAD    (float)(LOVE_M_PI / 180.0)
#define LOVE_M_TODEG    (float)(180.0 / LOVE_M_PI)
#define LOVE_TORAD(x)   (float)(x * LOVE_M_TORAD)
#define LOVE_TODEG(x)   (float)(x * LOVE_M_TODEG)

/* 3DS Texture Limits */
#define LOVE_TEX3DS_MIN (size_t)8
#define LOVE_TEX3DS_MAX (size_t)1024

namespace love
{
    struct Rect
    {
        static constexpr int EMPTY[4] = { -1, -1, -1, -1 };

        int x, y, w, h;

        Rect() : x(0), y(0), w(0), h(0)
        {}

        Rect(const int (&rect)[4])
        {
            this->x = rect[0];
            this->y = rect[1];
            this->w = rect[2];
            this->h = rect[3];
        }

        Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h)
        {}

        bool operator==(const Rect& other) const
        {
            return x == other.x && y == other.y && w == other.w && h == other.h;
        }
    };

    /*
     ** Clamps 3DS textures between min
     ** and max texture size to prevent
     ** the GPU from locking up
     ** ----
     ** credit: https://github.com/oreo639/3ds-theoraplayer/blob/master/source/frame.c#L10
     */
    static inline unsigned NextPo2(unsigned x)
    {
        if (x <= 2)
            return x;

        size_t result = 1U << (32 - __builtin_clz(x - 1));
        return std::clamp(result, LOVE_TEX3DS_MIN, LOVE_TEX3DS_MAX);
    }
} // namespace love
