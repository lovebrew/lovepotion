#pragma once

#include <algorithm>
#include <span>
#include <string_view>

#include <stdint.h>

namespace love
{
    struct ScreenInfo
    {
        uint8_t id;
        uint8_t index;
        std::string_view name;
        int width;
        int height;
    };

    enum Screen : int8_t;

    static constexpr inline Screen INVALID_SCREEN = (Screen)-1;
    static constexpr inline Screen DEFAULT_SCREEN = (Screen)0;
    inline Screen currentScreen                   = DEFAULT_SCREEN;

    std::span<const ScreenInfo> getScreenInfo();

    const ScreenInfo& getScreenInfo(Screen id);

    Screen getScreenId(const std::string& name);
} // namespace love
