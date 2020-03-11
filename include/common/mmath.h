#pragma once

#include <cstdlib> // for rand() and RAND_MAX

namespace love
{
    typedef struct Rect
    {
        int x, y;
        int w, h;

        bool operator == (const Rect & rhs) const
        {
            return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h;
        }
    } Rect;

    struct DrawArgs
    {
        float x = 0;
        float y = 0;

        float r = 0;

        float scalarX = 1;
        float scalarY = 1;

        float offsetX = 0;
        float offsetY = 0;

        float depth = 0;
    };

    inline int NextPO2(unsigned int in)
    {
        in--;
        in |= in >> 1;
        in |= in >> 2;
        in |= in >> 4;
        in |= in >> 8;
        in |= in >> 16;
        in++;

        return std::min(std::max(in, 8U), 1024U); // clamp size to keep gpu from locking
    }
}
