#pragma once

#include <stdint.h>
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
            CAFE
        };

        static constexpr std::string_view Name = __CONSOLE__;

        static constexpr Platform Current = (Name == "3DS") ? CTR : (Name == "Switch") ? HAC : CAFE;

        static constexpr bool is(Platform platform)
        {
            return Current == platform;
        }

        static void setMainCoreId(uint32_t id)
        {
            if (Console::mainCoreIdSet)
                return;

            Console::mainCoreId    = id;
            Console::mainCoreIdSet = true;
        }

        static uint32_t isMainCoreId(uint32_t id)
        {
            return Console::mainCoreId == id;
        }

        static uint32_t mainCoreId;
        static bool mainCoreIdSet;
    };
} // namespace love
