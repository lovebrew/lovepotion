#pragma once

#include <common/screen.hpp>

namespace love
{
    enum Screen : int8_t
    {
        DEFAULT
    };

    // clang-format off
    inline constinit ScreenInfo screenInfo[0x01] =
    {
        { Screen::DEFAULT, "default", -1, -1 },
    };
    // clang-format on

    inline void SetScreenSize(int width, int height)
    {
        auto& info = screenInfo[0];

        info.width  = width;
        info.height = height;
    }
} // namespace love
