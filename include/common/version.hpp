#pragma once

#include <array>
#include <compare>
#include <string>
#include <string_view>

namespace love
{
    struct Version
    {
      public:
        constexpr Version() : major(0), minor(0), revision(0)
        {}

        constexpr Version(std::string_view v) : major(0), minor(0), revision(0)
        {
            auto nposzero = [](size_t x) { return x == std::string_view::npos ? 0 : x; };

            auto major = v.substr(0, nposzero(v.find(DELIMITER)));

            size_t minorIdx = std::min(major.size() + 1, v.size());
            auto minor      = v.substr(minorIdx, nposzero(v.find(DELIMITER, minorIdx)));

            size_t microIdx = std::min(major.size() + 1 + minor.size() + 1, v.size());
            auto micro      = v.substr(microIdx, nposzero(v.find_first_not_of(DIGITS, microIdx)));

            if (major.size() != 0)
                this->major = atoi(major);

            if (minor.size() != 0)
                this->minor = atoi(minor);

            if (micro.size() != 0)
                this->revision = atoi(micro);

            if (major.size() == 0 && minor.size() == 0 && micro.size() == 0)
                this->major = atoi(v);
        }

        template<typename T>
        requires std::constructible_from<std::string_view, T> || std::convertible_to<T, std::string_view>
        constexpr Version(T&& version) : Version(std::string_view(std::forward<T>(version)))
        {}

        Version(uint8_t major, uint8_t minor, uint8_t micro) : major(major), minor(minor), revision(micro)
        {}

        constexpr std::strong_ordering operator<=>(const Version&) const noexcept = default;

        uint8_t major;
        uint8_t minor;
        uint8_t revision;

      private:
        static constexpr char DELIMITER     = '.';
        static constexpr const char* DIGITS = "123457890";

        constexpr static uint8_t atoi(std::string_view view)
        {
            uint8_t value = 0;

            for (const auto& character : view)
            {
                if ('0' <= character && character <= '9')
                    value = value * 10 + character - '0';
            }

            return value;
        }
    };

    static constexpr Version LOVE_POTION(__APP_VERSION__);
    static constexpr Version LOVE_FRAMEWORK(__LOVE_VERSION__);

    static constexpr const char* CODENAME                     = "Bestest Friend";
    static constexpr std::array<const char*, 1> COMPATIBILITY = { __LOVE_VERSION__ };
} // namespace love
