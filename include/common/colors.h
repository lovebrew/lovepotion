#pragma once

#include "common/runtime.h"
#include "common/logger.h"

template <typename T>
struct ColorT
{
    T r;
    T g;
    T b;
    T a;

    ColorT() : r(0), g(0), b(0), a(0) {}
    ColorT(T r_, T g_, T b_, T a_) : r(r_),
                                     g(g_),
                                     b(b_),
                                     a(a_)
    {}

    void Set(T r_, T g_, T b_, T a_)
    {
        r = r_;
        g = g_;
        b = b_;
        a = a_;
    }

    void CopyTo(T color[4])
    {
        color[0] = r;
        color[1] = g;
        color[2] = b;
        color[3] = a;
    }

    bool operator == (const ColorT<T> & other) const;
    bool operator != (const ColorT<T> & other) const;

    ColorT<T> operator += (const ColorT<T> &other);
    ColorT<T> operator *= (const ColorT<T> &other);
    ColorT<T> operator *= (T s);
    ColorT<T> operator /= (T s);
};

template <typename T>
bool ColorT<T>::operator == (const ColorT<T> & other) const
{
    return r == other.r && g == other.g && b == other.b && a == other.a;
}

template <typename T>
bool ColorT<T>::operator != (const ColorT<T> & other) const
{
    return !(operator==(other));
}

template <typename T>
ColorT<T> ColorT<T>::operator += (const ColorT<T> & other)
{
    r += other.r;
    g += other.g;
    b += other.b;
    a += other.a;

    return *this;
}

template <typename T>
ColorT<T> ColorT<T>::operator *= (const ColorT<T> & other)
{
    r *= other.r;
    g *= other.g;
    b *= other.b;
    a *= other.a;

    return *this;
}

template <typename T>
ColorT<T> ColorT<T>::operator *= (T s)
{
    r *= s;
    g *= s;
    b *= s;
    a *= s;

    return *this;
}

template <typename T>
ColorT<T> ColorT<T>::operator /= (T s)
{
    r /= s;
    g /= s;
    b /= s;
    a /= s;

    return *this;
}

template <typename T>
ColorT<T> operator + (const ColorT<T> & a, const ColorT<T> & b)
{
    ColorT<T> tmp(a);

    return tmp += b;
}

template <typename T>
ColorT<T> operator * (const ColorT<T> & a, const ColorT<T> & b)
{
    ColorT<T> res;
    res.r = a.r * b.r;
    res.g = a.g * b.g;
    res.b = a.b * b.b;
    res.a = a.a * b.a;

    return res;
}

template <typename T>
ColorT<T> operator * (const ColorT<T> & a, T s)
{
    ColorT<T> tmp(a);

    return tmp *= s;
}

template <typename T>
ColorT<T> operator / (const ColorT<T> & a, T s)
{
    ColorT<T> tmp(a);

    return tmp /= s;
}

typedef ColorT<unsigned char> Color32;
typedef ColorT<float> Colorf;

inline Color32 toColor32(Colorf cf)
{
    return Color32((unsigned char)(cf.r * 255.0f),
                   (unsigned char)(cf.g * 255.0f),
                   (unsigned char)(cf.b * 255.0f),
                   (unsigned char)(cf.a * 255.0f));
}

inline Colorf toColorf(Color32 c)
{
    return Colorf(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f);
}

inline void debugColor(Colorf color)
{
    LOG("{%.1f, %.1f, %.1f, %.1f}", color.r, color.g, color.b, color.a);
}