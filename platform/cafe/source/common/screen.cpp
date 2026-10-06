#include "common/screen.hpp"

#include <array>

#include <gx2/display.h>

namespace love
{
    // clang-format off
    static constexpr std::array<ScreenInfo, 2> TV_480P =
    {
        ScreenInfo{ GX2_SCAN_TARGET_TV,  0, "tv",      640, 480 },
        ScreenInfo{ GX2_SCAN_TARGET_DRC, 1, "gamepad", 854, 480 }
    };

    static constexpr std::array<ScreenInfo, 2> TV_480I =
    {
        ScreenInfo{ GX2_SCAN_TARGET_TV,  0, "tv",      720, 480 },
        ScreenInfo{ GX2_SCAN_TARGET_DRC, 1, "gamepad", 854, 480 }
    };

    static constexpr std::array<ScreenInfo, 2> TV_576I = 
    {
        ScreenInfo{ GX2_SCAN_TARGET_TV,  0, "tv",      768, 576 },
        ScreenInfo{ GX2_SCAN_TARGET_DRC, 1, "gamepad", 854, 480 }
    };

    static constexpr std::array<ScreenInfo, 2> TV_WIDE_720P =
    {
        ScreenInfo{ GX2_SCAN_TARGET_TV,  0, "tv",      1280, 720 },
        ScreenInfo{ GX2_SCAN_TARGET_DRC, 1, "gamepad", 854,  480 }
    };

    static constexpr std::array<ScreenInfo, 2> TV_WIDE_1080P =
    {
        ScreenInfo{ GX2_SCAN_TARGET_TV,  0, "tv",      1920, 1080 },
        ScreenInfo{ GX2_SCAN_TARGET_DRC, 1, "gamepad", 854,  480  }
    };
    // clang-format on

    std::span<const ScreenInfo> getScreenInfo()
    {
        switch (GX2GetSystemTVScanMode())
        {
            case GX2_TV_SCAN_MODE_576I:
                return TV_576I;
            case GX2_TV_SCAN_MODE_480I:
                return TV_480I;
            case GX2_TV_SCAN_MODE_480P:
                return TV_480P;
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
