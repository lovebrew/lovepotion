#pragma once

#include <array>
#include <compare>
#include <cstdint>
#include <string_view>

namespace love
{
    struct Version
    {
        uint8_t major = 0;
        uint8_t minor = 0;
        uint8_t patch = 0;

        constexpr Version() = default;

        constexpr Version(uint8_t major, uint8_t minor, uint8_t patch) :
            major(major),
            minor(minor),
            patch(patch)
        {}

        constexpr Version(std::string_view view)
        {
            this->major = parse_u8(view);
            if (!view.empty() && view.front() == '.')
                this->minor = parse_u8(view);

            if (!view.empty() && view.front() == '.')
                this->patch = parse_u8(view);
        }

        constexpr std::strong_ordering operator<=>(const Version&) const noexcept = default;

      private:
        static constexpr uint8_t parse_u8(std::string_view& s)
        {
            uint32_t value = 0;
            while (!s.empty() && s.front() >= '0' && s.front() <= '9')
                value = value * 10 + (s.front() - '0'), s.remove_prefix(1);
            return value;
        }
    };

    static constexpr Version POTION_VERSION(LOVE_POTION_VERSION_STRING);
    static constexpr Version VERSION(LOVE_VERSION_STRING);

    static constexpr const char* VERSION_CODENAME                     = "Bestest Friend";
    static constexpr std::array<const char*, 1> VERSION_COMPATIBILITY = { LOVE_VERSION_STRING };
} // namespace love
