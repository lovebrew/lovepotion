#pragma once

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

        Console() = delete;
    };
} // namespace love
