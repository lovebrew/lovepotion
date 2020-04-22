#pragma once

#include "common/runtime.h"
#include "common/logger.h"

struct Color
{
    float r;
    float g;
    float b;
    float a = 1.0f;
};

namespace Colors
{
    inline Color ALPHA_BLEND_COLOR(Color src, Color dst)
    {
        Color res = Color({1, 1, 1, 1});

        res.r = dst.r * (1.0f - src.a) + src.r;
        res.g = dst.g * (1.0f - src.a) + src.g;
        res.b = dst.b * (1.0f - src.a) + src.b;
        res.a = dst.a * (1.0f - src.a) + src.a;

        return res;
    }

    inline void DEBUG_COLOR(const Color  & color)
    {
        LOG("{%.2f, %.2f, %.2f, %.2f}", color.r, color.g, color.b, color.a);
    }
}
