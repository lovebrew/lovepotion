#pragma once

#define _USE_MATH_DEFINES
#include <algorithm>
#include <cstdlib> // for rand() and RAND_MAX
#include <math.h>

/* PI Constants */
#define LOVE_M_PI       3.14159265358979323846
#define LOVE_M_PI_2     1.57079632679489661923
#define LOVE_M_PI_4     0.785398163397448309616
#define LOVE_M_1_PI     0.318309886183790671538
#define LOVE_M_2_PI     0.636619772367581343076
#define LOVE_M_2_SQRTPI 1.12837916709551257390

/* Converting Degrees */
#define LOVE_M_TORAD  (float)(LOVE_M_PI / 180.0)
#define LOVE_M_TODEG  (float)(180.0 / LOVE_M_PI)
#define LOVE_TORAD(x) (float)(x * LOVE_M_TORAD)
#define LOVE_TODEG(x) (float)(x * LOVE_M_TODEG)

/* Misc. Math Constants */
#define LOVE_M_E      2.71828182845904523536
#define LOVE_M_LOG2E  1.44269504088896340736
#define LOVE_M_LOG10E 0.434294481903251827651
#define LOVE_M_LN2    0.693147180559945309417
#define LOVE_M_LN10   2.30258509299404568402

namespace love
{
    struct Rect
    {
        int x, y;
        int w, h;

        bool operator==(const Rect& rhs) const
        {
            return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h;
        }
    };

    constexpr size_t LOVE_MIN_TEX = 8U;
    constexpr size_t LOVE_MAX_TEX = 1024U;

    /*
    ** Clamps 3DS textures between min
    ** and max texture size to prevent
    ** the GPU from locking up
    */
    inline int NextPO2(size_t in)
    {
        in--;
        in |= in >> 1;
        in |= in >> 2;
        in |= in >> 4;
        in |= in >> 8;
        in |= in >> 16;
        in++;

        return std::clamp(in, LOVE_MIN_TEX, LOVE_MAX_TEX);
    }
} // namespace love
