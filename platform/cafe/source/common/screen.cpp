#include "common/screen.hpp"

#include <gx2/display.h>

namespace love
{
    // clang-format off
    inline constinit ScreenInfo TV_WIDE_480P[0x02] =
    {
        { GX2_SCAN_TARGET_TV,  0, "tv",      854, 480 },
        { GX2_SCAN_TARGET_DRC, 1, "gamepad", 854, 480 }
    };

    inline constinit ScreenInfo TV_WIDE_720P[0x02] =
    {
        { GX2_SCAN_TARGET_TV,  0, "tv",      1280, 720 },
        { GX2_SCAN_TARGET_DRC, 1, "gamepad", 854,  480 }
    };

    inline constinit ScreenInfo TV_WIDE_1080P[0x02] =
    {
        { GX2_SCAN_TARGET_TV,  0, "tv",      1920, 1080 },
        { GX2_SCAN_TARGET_DRC, 1, "gamepad", 854,  480  }
    };
    // clang-format on

    std::span<ScreenInfo> getScreenInfo()
    {
        switch (GX2GetSystemTVScanMode())
        {
            case GX2_TV_SCAN_MODE_480I:
            case GX2_TV_SCAN_MODE_480P:
                return TV_WIDE_480P;
            case GX2_TV_SCAN_MODE_720P:
                return TV_WIDE_720P;
            case GX2_TV_SCAN_MODE_1080I:
            case GX2_TV_SCAN_MODE_1080P:
                return TV_WIDE_1080P;
            case GX2_TV_SCAN_MODE_NONE:
            default:
                return {};
        }
    }
} // namespace love
