#pragma once

#include "common/Exception.hpp"

#include <compare>
#include <cstdint>

#include <algorithm>
#include <array>

namespace love
{
    struct Color
    {
      public:
        Color() : r(0), g(0), b(0), a(0)
        {}

        Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a)
        {}

        Color(const float (&rgba)[4]) : r(rgba[0]), g(rgba[1]), b(rgba[2]), a(rgba[3])
        {}

        Color(uint32_t abgr)
        {
            a = ((abgr >> 24) & 0xFF) / 255.0f;
            b = ((abgr >> 16) & 0xFF) / 255.0f;
            g = ((abgr >> 8) & 0xFF) / 255.0f;
            r = (abgr & 0xFF) / 255.0f;
        }

        constexpr std::strong_ordering operator<=>(const Color& other) const noexcept = default;

        Color operator+=(const Color& other)
        {
            this->r += other.r;
            this->g += other.g;
            this->b += other.b;
            this->a += other.a;

            return *this;
        }

        Color operator*=(const Color& other)
        {
            this->r *= other.r;
            this->g *= other.g;
            this->b *= other.b;
            this->a *= other.a;

            return *this;
        }

        Color operator/=(float divisor)
        {
            this->r /= divisor;
            this->g /= divisor;
            this->b /= divisor;
            this->a /= divisor;

            return *this;
        }

        uint32_t rgba() const
        {
            uint8_t _r = as_uint8_t(this->r);
            uint8_t _g = as_uint8_t(this->g);
            uint8_t _b = as_uint8_t(this->b);
            uint8_t _a = as_uint8_t(this->a);

            return _r | (_g << 8) | (_b << 16) | (_a << 24);
        }

        uint32_t abgr() const
        {
            uint8_t _r = as_uint8_t(this->r);
            uint8_t _g = as_uint8_t(this->g);
            uint8_t _b = as_uint8_t(this->b);
            uint8_t _a = as_uint8_t(this->a);

            return _a | (_b << 8) | (_g << 16) | (_r << 24);
        }

        std::array<float, 4> to_array() const
        {
            return { this->r, this->g, this->b, this->a };
        }

        float r, g, b, a;

      private:
        static uint8_t as_uint8_t(const float& in)
        {
            return (255.0f * std::clamp(in, 0.0f, 1.0f) + 0.5f);
        }
    };

    struct Color32
    {
      public:
        Color32() : r(0), g(0), b(0), a(0)
        {}

        Color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a)
        {}

        Color32(uint8_t (&rgba)[4]) : r(rgba[0]), g(rgba[1]), b(rgba[2]), a(rgba[3])
        {}

        constexpr std::strong_ordering operator<=>(const Color32& other) const noexcept = default;

        uint8_t r, g, b, a;
    };
} // namespace love
