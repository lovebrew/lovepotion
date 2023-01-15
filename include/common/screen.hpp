#pragma once

#include <span>
#include <string_view>
#include <vector>

#include <utilities/bidirectionalmap/smallvector.hpp>

#include <utilities/log/logfile.hpp>

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

    inline Screen currentScreen            = (Screen)0;
    static constexpr Screen SCREEN_INVALID = (Screen)-1;

    const ScreenInfo& GetScreenInfo(Screen);
    std::span<ScreenInfo> GetScreenInfo();

    inline bool IsActiveScreenValid()
    {
        return currentScreen != SCREEN_INVALID;
    }

    inline bool IsScreenActive(Screen id)
    {
        return currentScreen == id;
    }

    inline Screen GetActiveScreen()
    {
        return currentScreen;
    }

    inline void SetActiveScreen(Screen id)
    {
        currentScreen = id;
    }

    inline int GetScreenWidth(Screen id = currentScreen)
    {
        const auto& info = GetScreenInfo(id);

        return info.width;
    }

    inline int GetScreenHeight(Screen id = currentScreen)
    {
        const auto& info = GetScreenInfo(id);

        return info.height;
    }

    inline std::string_view GetScreenName(Screen id = currentScreen)
    {
        const auto& info = GetScreenInfo(id);

        return info.name;
    }

    inline std::string_view GetDefaultScreen()
    {
        return GetScreenName((Screen)0);
    }

    inline bool CheckScreenName(std::string_view name, Screen& screen)
    {
        const auto& info = GetScreenInfo();

        for (auto& value : info)
        {
            if (value.name == name)
            {
                screen = (Screen)value.id;
                return true;
            }
        }

        return false;
    }

    inline std::vector<std::string_view> GetScreens()
    {
        std::vector<std::string_view> result {};
        const auto& info = GetScreenInfo();

        for (auto& item : info)
            result.push_back(item.name);

        return result;
    }

    inline std::vector<Screen> GetScreenEnums()
    {
        std::vector<Screen> result {};
        const auto& info = GetScreenInfo();

        for (auto& item : info)
            result.push_back((Screen)item.id);

        return result;
    }
} // namespace love
