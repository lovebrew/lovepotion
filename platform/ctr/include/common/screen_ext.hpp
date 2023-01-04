#pragma once

#include <common/screen.hpp>

namespace love
{
    enum Screen : int8_t
    {
        LEFT,
        RIGHT,
        BOTTOM
    };

    // clang-format off
    static constexpr ScreenInfo screenInfo[0x03] =
    {
        { Screen::LEFT,   "left",   400, 240 },
        { Screen::RIGHT,  "right",  400, 240 },
        { Screen::BOTTOM, "bottom", 320, 240 }
    };

    static constexpr ScreenInfo altScreenInfo[0x02] =
    {
        { Screen::LEFT,   "top",    400, 240 },
        { Screen::BOTTOM, "bottom", 320, 240 }
    };

    static constexpr ScreenInfo wideScreenInfo[0x02] =
    {
        { Screen::LEFT,   "top",    800, 240 },
        { Screen::BOTTOM, "bottom", 320, 240 }
    };
    // clang-format on
} // namespace love
