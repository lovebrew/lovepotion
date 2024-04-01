#include "common/screen.hpp"

#include <algorithm>

#include <3ds.h>

namespace love
{
    // clang-format off
    inline constinit ScreenInfo screenInfo[0x03] =
    {
        { 0, "left",   400, 240 },
        { 1, "right",  320, 240 },
        { 2, "bottom", 320, 240 }
    };

    inline constinit ScreenInfo altScreenInfo[0x02] =
    {
        { 0, "top",    400, 240 },
        { 1, "bottom", 320, 240 }
    };

    inline constinit ScreenInfo wideScreenInfo[0x02] =
    {
        { 0, "top",    800, 240 },
        { 1, "bottom", 320, 240 }
    };
    // clang-format on

    std::span<ScreenInfo> getScreenInfo()
    {
        if (!gfxIs3D())
            return altScreenInfo;
        else if (gfxIsWide())
            return wideScreenInfo;

        return screenInfo;
    }
} // namespace love
