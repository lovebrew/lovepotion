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

        Console() = delete;
    };
} // namespace love
