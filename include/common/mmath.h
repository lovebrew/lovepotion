#pragma once

#include <cstdlib> // for rand() and RAND_MAX
#include <algorithm>

#define LOVE_M_TORAD	(float)(M_PI / 180.0f)
#define LOVE_M_TODEG    (float)(180.0f / M_PI)
#define LOVE_TORAD(x)	(float)(x * LOVE_M_TORAD)
#define LOVE_TODEG(x)	(float)(x * LOVE_M_TODEG)

namespace love
{
    typedef struct Rect
    {
        int x, y;
        int w, h;

        bool operator == (const Rect & rhs) const
        {
            return x == rhs.x && y == rhs.y &&
                   w == rhs.w && h == rhs.h;
        }
    } Rect;

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
}
