#include "common/screen.hpp"

#include <switch.h>

namespace love
{
    // clang-format off
    inline constinit ScreenInfo HANDHELD_MODE[0x01] =
    {
        { 0, 0, "default", 1280, 720 },
    };

    inline constinit ScreenInfo DOCKED_MODE[0x01] =
    {
        { 0, 0, "default", 1920, 1080 },
    };
    // clang-format on

    std::span<ScreenInfo> getScreenInfo()
    {
        if (appletGetOperationMode() == AppletOperationMode_Handheld)
            return HANDHELD_MODE;

        return DOCKED_MODE;
    }
} // namespace love
