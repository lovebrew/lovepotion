#pragma once

#include <algorithm>
#include <cstdint>
#include <ranges>
#include <span>
#include <type_traits>

template<typename T = void, std::size_t Size = 0>
class SmallTrivialVector;

template<std::default_initializable T, std::size_t Size>
requires(Size > 0)
class SmallTrivialVector<T, Size>
{
  private:
    std::array<T, Size> data {};
    std::size_t populated { 0 };

  public:
    consteval SmallTrivialVector() = default;

    consteval SmallTrivialVector(std::initializer_list<T> l) : data(), populated(0)
    {
        for (auto it = l.begin(); it != l.end() && populated < Size; ++it)
        {
            data[populated++] = *it;
        }

        for (std::size_t i = populated; i < Size; i++)
        {
            data[i] = T();
        }
    }

    template<typename... Args>
    consteval SmallTrivialVector(Args... args) : data { args... }, populated(sizeof...(Args))
    {}

    template<std::size_t SpanSize>
    requires(SpanSize != std::dynamic_extent && SpanSize <= Size)
    constexpr SmallTrivialVector(std::span<T, SpanSize> in) : data(), populated(SpanSize)
    {
        for (std::size_t i = 0; i < SpanSize; i++)
        {
            data[i] = in[i];
        }

        for (std::size_t i = populated; i < Size; i++)
        {
            data[i] = T();
        }
    }

    template<std::ranges::range Range>
    requires(std::is_convertible_v<std::ranges::range_value_t<Range>, T>)
    constexpr SmallTrivialVector(Range&& in) : data(), populated(0)
    {
        for (auto it = std::ranges::begin(in); it != std::ranges::end(in) && populated < Size; ++it)
        {
            data[populated++] = *it;
        }

        for (std::size_t i = populated; i < Size; i++)
        {
            data[i] = T();
        }
    }

    // Returns true if the value fit, false if it didn't
    constexpr bool push_back(const T& value)
    {
        if (size() < capacity())
        {
            data[populated++] = value;
            return true;
        }

        return false;
    }

    constexpr bool emplace_back(const T& value)
    {
        return push_back(value);
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

    constexpr std::size_t capacity() const
    {
        return Size;
    }

    constexpr typename std::array<T, Size>::iterator begin()
    {
        return data.begin();
    }

    constexpr typename std::array<T, Size>::iterator end()
    {
        return data.begin() + size();
    }

    constexpr typename std::array<T, Size>::reverse_iterator rbegin()
    {
        return data.rbegin() + (capacity() - size());
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
        return data.begin() + size();
    }

    constexpr typename std::array<T, Size>::const_reverse_iterator rbegin() const
    {
        return data.rbegin() + (capacity() - size());
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
        return data.begin() + size();
    }

    constexpr typename std::array<T, Size>::const_reverse_iterator crbegin() const
    {
        return data.rbegin() + (capacity() - size());
    }

    constexpr typename std::array<T, Size>::const_reverse_iterator crend() const
    {
        return data.rend();
    }
};

template<>
class SmallTrivialVector<>
{
  private:
    SmallTrivialVector() = default;

  public:
    template<typename T, std::size_t Size>
    static consteval SmallTrivialVector<T, Size> Create(std::span<T, Size> data)
    {
        return SmallTrivialVector<T, Size>(data);
    }
};

template<typename... Args>
SmallTrivialVector(Args...) -> SmallTrivialVector<std::common_type_t<Args...>, sizeof...(Args)>;
