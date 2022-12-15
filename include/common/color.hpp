#pragma once

#include "exception.hpp"
#include "vector.hpp"

#include <compare>
#include <stdint.h>

#include <algorithm>
#include <array>

struct Color
{
  public:
    Color() : r(0), g(0), b(0), a(0)
    {}

    Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a)
    {}

    /* only used for 3DS image data */
    Color(uint32_t abgr)
    {
        this->r = ((abgr & 0xFF000000) >> 0x18) / 255.0f;
        this->g = ((abgr & 0x00FF0000) >> 0x10) / 255.0f;
        this->b = ((abgr & 0x0000FF00) >> 0x08) / 255.0f;
        this->a = ((abgr & 0x000000FF) >> 0x00) / 255.0f;
    }

    constexpr std::strong_ordering operator<=>(const Color&) const noexcept = default;

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

    Color operator*=(float mul)
    {
        this->r *= mul;
        this->g *= mul;
        this->b *= mul;
        this->a *= mul;

        return *this;
    }

    Color operator/=(float div)
    {
        this->r /= div;
        this->g /= div;
        this->b /= div;
        this->a /= div;

        return *this;
    }

    /* https://github.com/devkitPro/citro2d/blob/master/include/c2d/base.h#L110 */
    uint32_t rgba() const
    {
        uint8_t red   = Color::to_uint8_t(this->r);
        uint8_t green = Color::to_uint8_t(this->g);
        uint8_t blue  = Color::to_uint8_t(this->b);
        uint8_t alpha = Color::to_uint8_t(this->a);

        return red | (green << (uint32_t)8) | (blue << (uint32_t)16) | (alpha << (uint32_t)24);
    }

    uint32_t abgr() const
    {
        uint8_t red   = Color::to_uint8_t(this->r);
        uint8_t green = Color::to_uint8_t(this->g);
        uint8_t blue  = Color::to_uint8_t(this->b);
        uint8_t alpha = Color::to_uint8_t(this->a);

        return alpha | (blue << (uint32_t)8) | (green << (uint32_t)16) | (red << (uint32_t)24);
    }

    /*
    ** For tex3ds-based textures
    ** @param data: data from the tex3ds texture
    ** @param width: power-of-two width of the data
    ** @param position: Vector2 coordinate inside the image ([0-width-1], [0-height-1])
    */
    static uint32_t* FromTile(const void* data, const unsigned width, love::Vector2 position)
    {
        return ((uint32_t*)data) + indexOfTile(width, position.x, position.y);
    }

    /*
    ** For tex3ds-based textures
    ** @param texture: C3D_Tex* holding texture data
    ** @param position: Vector2 coordinate inside the image ([0-width-1], [0-height-1])
    */
    template<typename T>
    static uint32_t* FromTile(const T* texture, love::Vector2 position)
    {
        return Color::FromTile(texture->data, texture->width, position);
    }

    float r;
    float g;
    float b;
    float a;

  private:
    static unsigned indexOfTile(const unsigned width, const unsigned x, const unsigned y)
    {
        const love::Vector2 tile(x / 8, y / 8);
        const love::Vector2 sub(x % 8, y % 8);

        if ((sub.y * 8 + sub.x) > coordsTable.size())
            throw love::Exception("Out of bounds tile position: %dx%d", sub.x, sub.y);

        return ((width / 8) * tile.y + tile.x) * 64 + coordsTable[sub.y * 8 + sub.x];
    }

    static const inline std::array<unsigned, 0x40> coordsTable = {
        0,  1,  4,  5,  16, 17, 20, 21, 2,  3,  6,  7,  18, 19, 22, 23, 8,  9,  12, 13, 24, 25,
        28, 29, 10, 11, 14, 15, 26, 27, 30, 31, 32, 33, 36, 37, 48, 49, 52, 53, 34, 35, 38, 39,
        50, 51, 54, 55, 40, 41, 44, 45, 56, 57, 60, 61, 42, 43, 46, 47, 58, 59, 62, 63,
    };

    /* https://github.com/devkitPro/citro2d/blob/master/include/c2d/base.h#L86*/
    static uint8_t to_uint8_t(const float& in)
    {
        return (uint8_t)(255.0f * std::clamp(in, 0.0f, 1.0f) + 0.5f);
    }
};

static inline unsigned coordToIndex(unsigned const width_, unsigned const x_, unsigned const y_)
{
    static unsigned char const table[] = {
        0,  1,  4,  5,  16, 17, 20, 21, 2,  3,  6,  7,  18, 19, 22, 23, 8,  9,  12, 13, 24, 25,
        28, 29, 10, 11, 14, 15, 26, 27, 30, 31, 32, 33, 36, 37, 48, 49, 52, 53, 34, 35, 38, 39,
        50, 51, 54, 55, 40, 41, 44, 45, 56, 57, 60, 61, 42, 43, 46, 47, 58, 59, 62, 63,
    };

    unsigned const tileX = x_ / 8;
    unsigned const tileY = y_ / 8;
    unsigned const subX  = x_ % 8;
    unsigned const subY  = y_ % 8;

    return ((width_ / 8) * tileY + tileX) * 64 + table[subY * 8 + subX];
}
