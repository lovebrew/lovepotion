#pragma once

#include <compare>
#include <stdint.h>

#include <algorithm>

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

    float r;
    float g;
    float b;
    float a;

  private:
    /* https://github.com/devkitPro/citro2d/blob/master/include/c2d/base.h#L86*/
    static uint8_t to_uint8_t(const float& in)
    {
        return (uint8_t)(255.0f * std::clamp(in, 0.0f, 1.0f) + 0.5f);
    }
};