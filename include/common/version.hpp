#pragma once

#include <array>
#include <compare>

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

        constexpr Version(const char* v)
        {
            // clang-format off
            major = parse_number(v);
            if (*v == '.') ++v;
            minor = parse_number(v);
            if (*v == '.') ++v;
            patch = parse_number(v);
            // clang-format on
        }

        constexpr std::strong_ordering operator<=>(const Version&) const noexcept = default;

      private:
        static constexpr uint8_t parse_number(const char*& s)
        {
            uint8_t value = 0;
            while (*s >= '0' && *s <= '9')
                value = value * 10 + (*s++ - '0');

            return value;
        }
    };

    static constexpr Version POTION_VERSION(LOVE_POTION_VERSION_STRING);
    static constexpr Version VERSION(LOVE_VERSION_STRING);

    static constexpr const char* VERSION_CODENAME                     = "Bestest Friend";
    static constexpr std::array<const char*, 1> VERSION_COMPATIBILITY = { LOVE_VERSION_STRING };
} // namespace love
