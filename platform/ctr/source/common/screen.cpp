#include "common/screen.hpp"

#include <algorithm>
#include <array>

#include <3ds.h>

namespace love
{
    // clang-format off
    static constexpr std::array<ScreenInfo, 3> GFX_3D =
    {
        ScreenInfo{ GFX_TOP,    0, "left",   GSP_SCREEN_HEIGHT_TOP,    GSP_SCREEN_WIDTH },
        ScreenInfo{ GFX_TOP,    1, "right",  GSP_SCREEN_HEIGHT_TOP,    GSP_SCREEN_WIDTH },
        ScreenInfo{ GFX_BOTTOM, 2, "bottom", GSP_SCREEN_HEIGHT_BOTTOM, GSP_SCREEN_WIDTH }
    };

    static constexpr std::array<ScreenInfo, 2> GFX_2D =
    {
        ScreenInfo{ GFX_TOP,    0, "top",    GSP_SCREEN_HEIGHT_TOP,    GSP_SCREEN_WIDTH },
        ScreenInfo{ GFX_BOTTOM, 1, "bottom", GSP_SCREEN_HEIGHT_BOTTOM, GSP_SCREEN_WIDTH }
    };

    static constexpr std::array<ScreenInfo, 2> GFX_WIDE =
    {
        ScreenInfo{ GFX_TOP,    0, "top",    GSP_SCREEN_HEIGHT_TOP_2X, GSP_SCREEN_WIDTH },
        ScreenInfo{ GFX_BOTTOM, 1, "bottom", GSP_SCREEN_HEIGHT_BOTTOM, GSP_SCREEN_WIDTH }
    };
    // clang-format on

    std::span<const ScreenInfo> getScreenInfo()
    {
        if (!gfxIs3D())
            return GFX_2D;
        else if (gfxIsWide())
            return GFX_WIDE;

        return GFX_3D;
    }
} // namespace love
