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
        static constexpr float WHITE[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        static constexpr float BLACK[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

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

        /*
         ** For tex3ds-based textures
         ** @param data: data from the tex3ds texture
         ** @param width: power-of-two width of the data
         ** @param position: Vector2 coordinate inside the image ([0-width-1], [0-height-1])
         */
        template<typename T = uint32_t>
        static T* fromTile(const void* data, const unsigned width, int x, int y)
        {
            return ((T*)data) + indexOfTile(width, x, y);
        }

        /*
        ** For tex3ds-based textures
        ** @param texture: C3D_Tex* holding texture data
        ** @param position: Vector2 coordinate inside the image ([0-width-1], [0-height-1])
        */
        template<typename T, typename V = uint32_t>
        static V* fromTile(const T* texture, int x, int y)
        {
            return Color::fromTile<V>(texture->data, texture->width, x, y);
        }

      private:
        static unsigned indexOfTile(const unsigned width, const unsigned x, const unsigned y)
        {
            const auto tileX = x / 8;
            const auto tileY = y / 8;

            const auto subX = x % 8;
            const auto subY = y % 8;

            if ((subY * 8 + subX) > coordsTable.size())
                throw love::Exception("Out of bounds tile position: {:d}x{:d}", subX, subY);

            return ((width / 8) * tileY + tileX) * 64 + coordsTable[subY * 8 + subX];
        }

        static const inline std::array<unsigned, 0x40> coordsTable = {
            0,  1,  4,  5,  16, 17, 20, 21, 2,  3,  6,  7,  18, 19, 22, 23, 8,  9,  12, 13, 24, 25,
            28, 29, 10, 11, 14, 15, 26, 27, 30, 31, 32, 33, 36, 37, 48, 49, 52, 53, 34, 35, 38, 39,
            50, 51, 54, 55, 40, 41, 44, 45, 56, 57, 60, 61, 42, 43, 46, 47, 58, 59, 62, 63
        };

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

    static bool gammaCorrect = false;

    inline void setGammaCorrect(bool enable)
    {
        gammaCorrect = enable;
    }

    inline bool isGammaCorrect()
    {
        return gammaCorrect;
    }
} // namespace love
