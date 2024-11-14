#pragma once

#include <span>
#include <string_view>

#include <algorithm>

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

    std::span<ScreenInfo> getScreenInfo();

    const inline ScreenInfo& getScreenInfo(Screen id)
    {
        const auto& info = getScreenInfo();

        id = (Screen)std::clamp<int8_t>(id, 0, info.size() - 1);

        return info[id];
    }

    inline Screen getScreenId(std::string_view name)
    {
        const auto& info = getScreenInfo();

        for (size_t i = 0; i < info.size(); ++i)
        {
            if (info[i].name == name)
                return Screen(i);
        }

        return INVALID_SCREEN;
    }

    inline void setScreen(Screen id)
    {
        const auto& info = getScreenInfo();

        id = (Screen)std::clamp<int8_t>(id, 0, info.size() - 1);

        currentScreen = id;
    }
} // namespace love
