#include <common/screen_ext.hpp>

namespace love
{
    std::span<ScreenInfo> GetScreenInfo()
    {
        return { screenInfo };
    }

    const ScreenInfo& GetScreenInfo(Screen id)
    {
        const auto& info = GetScreenInfo();

        return info[id];
    }
} // namespace love
