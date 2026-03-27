#include "common/screen.hpp"

#include <array>

#include <switch.h>

namespace love
{
    // clang-format off
    static constexpr std::array<ScreenInfo, 1> HANDHELD_MODE =
    {
        { 0, 0, "default", 1280, 720 }
    };

    static constexpr std::array<ScreenInfo, 1> DOCKED_MODE =
    {
        { 0, 0, "default", 1920, 1080 }
    };
    // clang-format on

    std::span<const ScreenInfo> getScreenInfo()
    {
        if (appletGetOperationMode() == AppletOperationMode_Handheld)
            return HANDHELD_MODE;

        return DOCKED_MODE;
    }
} // namespace love
