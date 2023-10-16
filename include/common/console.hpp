#pragma once

#include <common/screen_ext.hpp>

#include <string_view>

namespace love
{
    class Console
    {
      public:
        enum Platform
        {
            CTR,
            HAC,
            CAFE,
            ALL
        };

        static constexpr std::string_view Name = __CONSOLE__;

        static constexpr Platform Which = (Name == "3DS") ? CTR : (Name == "Switch") ? HAC : CAFE;

        static constexpr bool Is(Console::Platform platform)
        {
            return Which == platform;
        }

        static constexpr bool IsBigEndian()
        {
            return Which == CAFE;
        }

        static void SetMainCore(uint32_t id)
        {
            if (Console::coreIdSet)
                return;

            Console::coreId    = id;
            Console::coreIdSet = true;
        }

        static uint32_t GetMainCoreId()
        {
            return Console::coreId;
        }

        static uint32_t coreId;
        static bool coreIdSet;

        Console() = delete;
    };
} // namespace love
