#pragma once

#include <array>
#include <cstdint>
#include <initializer_list>
#include <type_traits>

template<typename T, std::size_t Size>
requires std::is_trivial_v<T>
class SmallTrivialVector
{
  private:
    std::array<T, Size> data {};
    std::size_t populated { 0 };

  public:
    consteval SmallTrivialVector() = default;

    constexpr SmallTrivialVector(std::initializer_list<T> in) : data(), populated(in.size())
    {
        for (std::size_t i = 0; i < in.size(); i++)
        {
            data[i] = *(in.begin() + i);
        }
    }

    constexpr SmallTrivialVector(std::array<T, Size> in) : data(), populated(Size)
    {
        for (std::size_t i = 0; i < Size; i++)
        {
            data[i] = in[i];
        }
    }

    template<std::size_t ArraySize>
    constexpr SmallTrivialVector(std::array<T, ArraySize> in) : data(), populated(ArraySize)
    {
        for (std::size_t i = 0; i < ArraySize; i++)
        {
            data[i] = in[i];
        }
    }

    template<typename Iterator>
    requires requires(Iterator t)
    {
        {
            *t
            } -> std::same_as<const T&>;
    }
    constexpr SmallTrivialVector(Iterator begin, Iterator end) : data()
    {
        while (begin != end)
        {
            data[populated++] = *begin;
            ++begin;
        }
    }

    constexpr bool push_back(const T& value)
    {
        if (populated < Size)
        {
            data[i] = value;
            populated++;
            return true;
        }
        return false;
    }

    constexpr const T& operator[](std::size_t i) const
    {
        return data[i];
    }
    constexpr T& operator[](std::size_t i)
    {
        return data[i];
    }

    constexpr std::size_t size() const
    {
        return populated;
    }

    constexpr typename std::array<T, Size>::iterator begin()
    {
        return data.begin();
    }
    constexpr typename std::array<T, Size>::iterator end()
    {
        return data.begin() + populated;
    }
    constexpr typename std::array<T, Size>::reverse_iterator rbegin()
    {
        return data.rbegin() + (Size - populated);
    }
    constexpr typename std::array<T, Size>::reverse_iterator rend()
    {
        return data.rend();
    }
    constexpr typename std::array<T, Size>::const_iterator begin() const
    {
        return data.begin();
    }
    constexpr typename std::array<T, Size>::const_iterator end() const
    {
        return data.begin() + populated;
    }
    constexpr typename std::array<T, Size>::const_reverse_iterator rbegin() const
    {
        return data.rbegin() + (Size - populated);
    }
    constexpr typename std::array<T, Size>::const_reverse_iterator rend() const
    {
        return data.rend();
    }
    constexpr typename std::array<T, Size>::const_iterator cbegin() const
    {
        return data.begin();
    }
    constexpr typename std::array<T, Size>::const_iterator cend() const
    {
        return data.begin() + populated;
    }
    constexpr typename std::array<T, Size>::const_reverse_iterator crbegin() const
    {
        return data.rbegin() + (Size - populated);
    }
    constexpr typename std::array<T, Size>::const_reverse_iterator crend() const
    {
        return data.rend();
    }
};
