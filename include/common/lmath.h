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

#define LOVE_TEX3DS_MIN 8U
#define LOVE_TEX3DS_MAX 1024U

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

    /*
    ** Clamps 3DS textures between min
    ** and max texture size to prevent
    ** the GPU from locking up
    ** ----
    ** credit: https://github.com/oreo639/3ds-theoraplayer/blob/master/source/frame.c#L10
    */
    static inline uint32_t NextPo2(uint32_t x)
    {
        if (x <= 2)
            return x;

        uint32_t result = 1u << (32 - __builtin_clz(x - 1));
        return std::clamp(result, LOVE_TEX3DS_MIN, LOVE_TEX3DS_MAX);
    }
} // namespace love
