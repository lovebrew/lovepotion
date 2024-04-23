#include "common/screen.hpp"

#include <algorithm>

#include <3ds.h>

namespace love
{
    // clang-format off
    inline constinit ScreenInfo GFX_3D[0x03] =
    {
        { GFX_TOP,    0, "left",   400, 240 },
        { GFX_TOP,    1, "right",  400, 240 },
        { GFX_BOTTOM, 2, "bottom", 320, 240 }
    };

    inline constinit ScreenInfo GFX_2D[0x02] =
    {
        { GFX_TOP,    0, "top",    400, 240 },
        { GFX_BOTTOM, 1, "bottom", 320, 240 }
    };

    inline constinit ScreenInfo GFX_WIDE[0x02] =
    {
        { GFX_TOP,    0, "top",    800, 240 },
        { GFX_BOTTOM, 1, "bottom", 320, 240 }
    };
    // clang-format on

    std::span<ScreenInfo> getScreenInfo()
    {
        if (!gfxIs3D())
            return GFX_2D;
        else if (gfxIsWide())
            return GFX_WIDE;

        return GFX_3D;
    }
} // namespace love
