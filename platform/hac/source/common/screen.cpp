#include "common/screen.hpp"

#include <switch.h>

namespace love
{
    // clang-format off
    inline constinit ScreenInfo screenInfo[0x01] =
    {
        { 0, "default", -1, -1 },
    };
    // clang-format on

    std::span<ScreenInfo> getScreenInfo()
    {
        return screenInfo;
    }

    inline void setScreenSize(int width, int height)
    {
        auto& info = screenInfo[0];

        info.width  = width;
        info.height = height;
    }
} // namespace love
