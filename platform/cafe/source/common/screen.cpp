#include "common/screen.hpp"

namespace love
{
    // clang-format off
    inline constinit ScreenInfo screenInfo[0x02] =
    {
        { 0, "tv",      -1, -1 },
        { 1, "gamepad", -1, -1 }
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
