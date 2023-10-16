#pragma once

#include <algorithm>
#include <compare>
#include <cstdint>
#include <iterator>
#include <memory>
#include <span>
#include <type_traits>

namespace SmallVectorInternals
{
    template<typename T>
    union STVData
    {
        T a;
        unsigned char b;
        constexpr STVData() : b()
        {}
        constexpr ~STVData()
        {}
    };

    template<typename T>
    struct STVIterator
    {
      private:
        using data_type =
            std::conditional_t<std::is_const_v<T>, const STVData<std::remove_cv_t<T>>, STVData<T>>;
        data_type* data;

      public:
        constexpr STVIterator() = default;

        constexpr explicit STVIterator(data_type* d) : data(d)
        {}

        using difference_type   = decltype(data - data);
        using value_type        = T;
        using pointer           = T*;
        using const_pointer     = const T*;
        using reference         = value_type&;
        using const_reference   = const value_type&;
        using iterator_category = std::random_access_iterator_tag;
        using iterator_concept  = std::random_access_iterator_tag;

        constexpr reference operator*() noexcept
        {
            return data->a;
        }
        constexpr const_reference operator*() const noexcept
        {
            return data->a;
        }
        constexpr pointer operator->() noexcept
        {
            return &data->a;
        }
        constexpr const_pointer operator->() const noexcept
        {
            return &data->a;
        }
        constexpr reference operator[](difference_type i) noexcept
        {
            return (data + i)->a;
        }
        constexpr const_reference operator[](difference_type i) const noexcept
        {
            return (data + i)->a;
        }

        constexpr bool operator==(const STVIterator&) const noexcept  = default;
        constexpr auto operator<=>(const STVIterator&) const noexcept = default;

        constexpr STVIterator& operator++() noexcept
        {
            data++;
            return *this;
        }
        constexpr STVIterator operator++(int) noexcept
        {
            STVIterator ret;
            data++;
            return ret;
        }
        constexpr STVIterator& operator--() noexcept
        {
            data--;
            return *this;
        }
        constexpr STVIterator operator--(int) noexcept
        {
            STVIterator ret;
            data--;
            return ret;
        }

        constexpr difference_type operator-(const STVIterator& o) const noexcept
        {
            return data - o.data;
        }

        constexpr STVIterator& operator+=(difference_type d) noexcept
        {
            data += d;
            return *this;
        }

        constexpr STVIterator& operator-=(difference_type d) noexcept
        {
            data -= d;
            return *this;
        }
    };
} // namespace SmallVectorInternals

template<typename T>
constexpr SmallVectorInternals::STVIterator<T> operator+(
    const SmallVectorInternals::STVIterator<T>& it,
    typename SmallVectorInternals::STVIterator<T>::difference_type i)
{
    auto ret = it;
    ret += i;
    return ret;
}

template<typename T>
constexpr SmallVectorInternals::STVIterator<T> operator+(
    typename SmallVectorInternals::STVIterator<T>::difference_type i,
    const typename SmallVectorInternals::STVIterator<T>& it)
{
    auto ret = it;
    ret += i;
    return ret;
}

template<typename T>
constexpr SmallVectorInternals::STVIterator<T> operator-(
    const SmallVectorInternals::STVIterator<T>& it,
    typename SmallVectorInternals::STVIterator<T>::difference_type i)
{
    auto ret = it;
    ret -= i;
    return ret;
}

template<typename T, std::size_t Size>
requires std::is_destructible_v<T>
class SmallVector
{
  private:
    static_assert(sizeof(SmallVectorInternals::STVData<T>) == sizeof(T),
                  "A union of a char and a T was larger than T. Why is your compiler bad?");

    std::array<SmallVectorInternals::STVData<T>, Size> alldata;
    std::size_t populated { 0 };

    constexpr SmallVector& self()
    {
        return *this;
    }
    constexpr const SmallVector& self() const
    {
        return *this;
    }

  public:
    using value_type             = T;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = value_type*;
    using const_pointer          = const value_type*;
    using iterator               = SmallVectorInternals::STVIterator<value_type>;
    using const_iterator         = SmallVectorInternals::STVIterator<const value_type>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr SmallVector() = default;

    constexpr ~SmallVector() noexcept(std::is_nothrow_destructible_v<T>)
    {
        for (std::size_t i = 0; i < size(); i++)
        {
            std::destroy_at(&self()[i]);
        }
    }

    constexpr SmallVector(const SmallVector& o) noexcept(noexcept(push_back(o[0])))
    requires std::is_copy_constructible_v<T>
    {
        for (std::size_t i = 0; i < o.size(); i++)
        {
            push_back(o[i]);
        }
    }

    constexpr SmallVector(SmallVector&& o) noexcept(noexcept(emplace_back(std::move(o[0]))) &&
                                                    std::is_nothrow_destructible_v<T>)
    requires std::is_move_constructible_v<T>
    {
        for (std::size_t i = 0; i < o.size(); i++)
        {
            emplace_back(std::move(o[i]));
            std::destroy_at(&o[i]);
        }
        o.populated = 0;
    }

    constexpr SmallVector& operator=(const SmallVector& o) noexcept(
        std::is_nothrow_destructible_v<T>&& std::is_nothrow_copy_constructible_v<T>&&
            std::is_nothrow_copy_assignable_v<T>)
    requires std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>
    {
        std::size_t minsize     = std::min(size(), o.size());
        std::size_t targetsize  = o.size();
        std::size_t currentsize = size();

        for (std::size_t i = 0; i < minsize; i++)
        {
            self()[i] = o[i];
        }
        for (std::size_t i = minsize; i < targetsize; i++)
        {
            std::construct_at(&self()[i], o[i]);
        }
        for (std::size_t i = targetsize; i < currentsize; i++)
        {
            std::destroy_at(&self()[i]);
        }

        populated = o.populated;

        return *this;
    }

    constexpr SmallVector& operator=(SmallVector&& o) noexcept(
        std::is_nothrow_destructible_v<T>&& std::is_nothrow_move_constructible_v<T>&&
            std::is_nothrow_move_assignable_v<T>)
    requires std::is_move_constructible_v<T> || std::is_move_assignable_v<T>
    {
        std::size_t minsize     = std::min(size(), o.size());
        std::size_t targetsize  = o.size();
        std::size_t currentsize = size();

        for (std::size_t i = 0; i < minsize; i++)
        {
            self()[i] = std::move(o[i]);
            std::destroy_at(&o[i]);
        }
        for (std::size_t i = minsize; i < targetsize; i++)
        {
            std::construct_at(&self()[i], std::move(o[i]));
            std::destroy_at(&o[i]);
        }
        for (std::size_t i = targetsize; i < currentsize; i++)
        {
            std::destroy_at(self()[i]);
        }

        populated   = o.populated;
        o.populated = 0;

        return *this;
    }

    template<typename... Args>
    requires(std::convertible_to<Args, T> && ...) && (sizeof...(Args) <= Size)
    constexpr SmallVector(Args&&... args) noexcept(
        (noexcept(emplace_back(std::forward<decltype(args)>(args))) && ...))
    {
        (emplace_back(std::forward<decltype(args)>(args)) && ...);
    }

    template<std::size_t SpanSize, typename Contained>
    requires std::convertible_to<Contained, T> && (SpanSize <= Size) &&
             (SpanSize != std::dynamic_extent)
    constexpr SmallVector(const std::span<Contained, SpanSize>& in) noexcept(
        noexcept(emplace_back(in[0])))
    {
        for (std::size_t i = 0; i < in.size(); i++)
        {
            emplace_back(in[i]);
        }
    }

    // Returns true if the value fit, false if it didn't
    constexpr bool push_back(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
    requires std::is_copy_constructible_v<T>
    {
        if (size() < capacity())
        {
            std::construct_at(&self()[populated++], std::forward<const T&>(value));
            return true;
        }

        return false;
    }

    template<typename... Args>
    constexpr bool emplace_back(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, decltype(args)...>)
    requires std::is_constructible_v<T, decltype(args)...>
    {
        if (size() < capacity())
        {
            std::construct_at(&self()[populated++], std::forward<decltype(args)>(args)...);
            return true;
        }

        return false;
    }

    constexpr void pop_back() noexcept(std::is_nothrow_destructible_v<T>)
    {
        if (size() > 0)
        {
            std::destroy_at(&self()[--populated]);
        }
    }

    constexpr const_reference operator[](size_type i) const noexcept
    {
        return alldata[i].a;
    }

    constexpr reference operator[](size_type i) noexcept
    {
        return alldata[i].a;
    }

    constexpr size_type size() const noexcept
    {
        return populated;
    }

    constexpr size_type capacity() const noexcept
    {
        return Size;
    }

    constexpr iterator begin() noexcept
    {
        return iterator { alldata.data() };
    }

    constexpr iterator end() noexcept
    {
        return iterator { alldata.data() } + size();
    }

    constexpr reverse_iterator rbegin() noexcept
    {
        return std::make_reverse_iterator(end());
    }

    constexpr reverse_iterator rend() noexcept
    {
        return std::make_reverse_iterator(begin());
    }

    constexpr const_iterator begin() const noexcept
    {
        return const_iterator { alldata.data() };
    }

    constexpr const_iterator end() const noexcept
    {
        return const_iterator { alldata.data() } + size();
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return std::make_reverse_iterator(end());
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return std::make_reverse_iterator(begin());
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return const_iterator { alldata.data() };
    }

    constexpr const_iterator cend() const noexcept
    {
        return const_iterator { alldata.data() } + size();
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return std::make_reverse_iterator(cend());
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return std::make_reverse_iterator(cbegin());
    }
};

template<typename... Args>
SmallVector(Args...) -> SmallVector<std::common_type_t<Args...>, sizeof...(Args)>;
template<typename T, std::size_t Size>
SmallVector(std::span<T, Size>) -> SmallVector<T, Size>;
