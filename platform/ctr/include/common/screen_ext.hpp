#pragma once

#include <3ds.h>

#include <span>

namespace love
{
    enum Screen
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

    std::span<const ScreenInfo> GetScreenInfo()
    {
        if (!gfxIs3D())
            return { altScreenInfo };
        else if (gfxIsWide())
            return { wideScreenInfo };
        else
            return { screenInfo };
    }

    const ScreenInfo& GetScreenInfo(Screen id)
    {
        const auto& info = GetScreenInfo();

        return info[id];
    }
} // namespace love
