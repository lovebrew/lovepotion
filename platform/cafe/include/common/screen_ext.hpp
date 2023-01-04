#pragma once

#include <common/screen.hpp>

namespace love
{
    enum Screen : int8_t
    {
        TV,
        GAMEPAD
    };

    // clang-format off
    static constinit ScreenInfo screenInfo[0x02] =
    {
        { Screen::TV,      "tv",      -1, -1 },
        { Screen::GAMEPAD, "gamepad", -1, -1 }
    };
    // clang-format on

    void SetScreenSize(Screen id, int width, int height)
    {
        auto& info = screenInfo[id];

        info.width  = width;
        info.height = height;
    }
} // namespace love
