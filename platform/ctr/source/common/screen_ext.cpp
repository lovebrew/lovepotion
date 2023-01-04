#include <common/screen_ext.hpp>

#include <3ds.h>

namespace love
{
    std::span<ScreenInfo> GetScreenInfo()
    {
        if (!gfxIs3D())
            return { altScreenInfo };
        else if (gfxIsWide())
            return { wideScreenInfo };
        else
            return { screenInfo };
    }

    const ScreenInfo& GetScreenInfo(Screen id)
    {
        const auto& info = GetScreenInfo();

        return info[id];
    }
} // namespace love
