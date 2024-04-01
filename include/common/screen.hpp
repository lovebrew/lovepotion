#pragma once

#include <span>
#include <string_view>

#include <algorithm>

#include <stdint.h>

namespace love
{
    struct ScreenInfo
    {
        int8_t id;
        std::string_view name;
        int width;
        int height;
    };

    enum Screen : int8_t;

    static constexpr inline Screen INVALID_SCREEN = (Screen)-1;
    static constexpr inline Screen DEFAULT_SCREEN = (Screen)0;
    inline Screen currentScreen                   = INVALID_SCREEN;

    std::span<ScreenInfo> getScreenInfo();

    const inline ScreenInfo& getScreenInfo(Screen id)
    {
        const auto& info = getScreenInfo();

        id = (Screen)std::clamp<int8_t>(id, 0, info.size() - 1);

        return info[id];
    }
} // namespace love
