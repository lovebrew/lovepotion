#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>

namespace love
{
    template<typename T>
    struct ColorT
    {
      public:
        T r { 0 }, g { 0 }, b { 0 }, a { 0 };

        constexpr ColorT() noexcept = default;

        constexpr ColorT(T r, T g, T b, T a) noexcept : r(r), g(g), b(b), a(a)
        {}

        constexpr ColorT(const Color& other) noexcept : r(other.r), g(other.g), b(other.b), a(other.a)
        {}

        constexpr explicit ColorT(uint32_t abgr) noexcept
        {
            this->a = ((abgr >> 24) & 0xFF) / 255.0f;
            this->b = ((abgr >> 16) & 0xFF) / 255.0f;
            this->g = ((abgr >> 8) & 0xFF) / 255.0f;
            this->r = (abgr & 0xFF) / 255.0f;
        }

        uint8_t abgr() const
        {
            uint8_t _r = as_uint8_t(this->r);
            uint8_t _g = as_uint8_t(this->g);
            uint8_t _b = as_uint8_t(this->b);
            uint8_t _a = as_uint8_t(this->a);

            return _a | (_b << 8) | (_g << 16) | (_r << 24);
        }

        bool operator==(const ColorT<T>& other) const;
        bool operator!=(const ColorT<T>& other) const;

        ColorT<T> operator+=(const ColorT<T>& other);
        ColorT<T> operator*=(const ColorT<T>& other);
        ColorT<T> operator*=(T s);
        ColorT<T> operator/=(T s);

      private:
        static uint8_t as_uint8_t(const float& in)
        {
            return (255.0f * std::clamp(in, 0.0f, 1.0f) + 0.5f);
        }
    };

    template<typename T>
    bool ColorT<T>::operator==(const ColorT<T>& other) const
    {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    template<typename T>
    bool ColorT<T>::operator!=(const ColorT<T>& other) const
    {
        return !(operator==(other));
    }

    template<typename T>
    ColorT<T> ColorT<T>::operator+=(const ColorT<T>& other)
    {
        this->r += other.r;
        this->g += other.g;
        this->b += other.b;
        this->a += other.a;
        return *this;
    }

    template<typename T>
    ColorT<T> ColorT<T>::operator*=(const ColorT<T>& other)
    {
        this->r *= other.r;
        this->g *= other.g;
        this->b *= other.b;
        this->a *= other.a;
        return *this;
    }

    template<typename T>
    ColorT<T> ColorT<T>::operator*=(T s)
    {
        this->r *= s;
        this->g *= s;
        this->b *= s;
        this->a *= s;
        return *this;
    }

    template<typename T>
    ColorT<T> ColorT<T>::operator/=(T s)
    {
        this->r /= s;
        this->g /= s;
        this->b /= s;
        this->a /= s;
        return *this;
    }

    template<typename T>
    ColorT<T> operator+(const ColorT<T>& a, const ColorT<T>& b)
    {
        ColorT<T> tmp(a);
        return tmp += b;
    }

    template<typename T>
    ColorT<T> operator*(const ColorT<T>& a, const ColorT<T>& b)
    {
        ColorT<T> res;
        res.r = a.r * b.r;
        res.g = a.g * b.g;
        res.b = a.b * b.b;
        res.a = a.a * b.a;
        return res;
    }

    template<typename T>
    ColorT<T> operator*(const ColorT<T>& a, T s)
    {
        ColorT<T> tmp(a);
        return tmp *= s;
    }

    template<typename T>
    ColorT<T> operator/(const ColorT<T>& a, T s)
    {
        ColorT<T> tmp(a);
        return tmp /= s;
    }

    using Color   = ColorT<float>;
    using Color32 = ColorT<uint8_t>;
    using ColorD  = ColorT<double>;

    // clang-format off
    static constexpr inline uint32_t coordinates[0x40] = {
        0,  1,  4,  5,  16, 17, 20, 21, 2,  3,  6,  7,  18,
        19, 22, 23, 8,  9,  12, 13, 24, 25, 28, 29, 10, 11,
        14, 15, 26, 27, 30, 31, 32, 33, 36, 37, 48, 49, 52,
        53, 34, 35, 38, 39, 50, 51, 54, 55, 40, 41, 44, 45,
        56, 57, 60, 61, 42, 43, 46, 47, 58, 59, 62, 63
    };
    // clang-format on

    static unsigned indexOfTile(const unsigned width, const unsigned x, const unsigned y)
    {
        const auto tile_x = x / 8, tile_y = y / 8;
        const auto subtile_x = x % 8, subtile_y = y % 8;

        return ((width / 8) * tile_y + tile_x) * 64 + coordinates[subtile_y * 8 + subtile_x];
    }

    template<typename T = uint32_t>
    inline T* colorFromTile(const void* data, const unsigned width, int x, int y)
    {
        return ((T*)data) + indexOfTile(width, x, y);
    }

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
