#include "common/screen.hpp"

#include <algorithm>

#include <3ds.h>

namespace love
{
    // clang-format off
    inline constinit ScreenInfo GFX_3D[0x03] =
    {
        { GFX_TOP,    0, "left",   GSP_SCREEN_HEIGHT_TOP,    GSP_SCREEN_WIDTH },
        { GFX_TOP,    1, "right",  GSP_SCREEN_HEIGHT_TOP,    GSP_SCREEN_WIDTH },
        { GFX_BOTTOM, 2, "bottom", GSP_SCREEN_HEIGHT_BOTTOM, GSP_SCREEN_WIDTH }
    };

    inline constinit ScreenInfo GFX_2D[0x02] =
    {
        { GFX_TOP,    0, "top",    GSP_SCREEN_HEIGHT_TOP,    GSP_SCREEN_WIDTH },
        { GFX_BOTTOM, 1, "bottom", GSP_SCREEN_HEIGHT_BOTTOM, GSP_SCREEN_WIDTH }
    };

    inline constinit ScreenInfo GFX_WIDE[0x02] =
    {
        { GFX_TOP,    0, "top",    GSP_SCREEN_HEIGHT_TOP_2X, GSP_SCREEN_WIDTH },
        { GFX_BOTTOM, 1, "bottom", GSP_SCREEN_HEIGHT_BOTTOM, GSP_SCREEN_WIDTH }
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
