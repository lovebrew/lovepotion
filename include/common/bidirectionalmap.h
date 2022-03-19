#pragma once

#include <array>
#include <functional>
#include <string_view>
#include <type_traits>
#include <utility>

// Note: both Key and Value must have a default constructor
// Note: KeyComparator and ValueComparator must act similarly to std::equal_to
template<typename K = void, typename V = void, size_t Size = 0, typename KC = std::equal_to<>,
         typename VC = std::equal_to<>>
class BidirectionalMap
{
    static_assert(Size > 0);
    static_assert(!std::is_same_v<K, void>);
    static_assert(!std::is_same_v<V, void>);

  private:
    template<size_t ArraySize>
    constexpr void populate(const std::pair<K, V>* data)
    {
        static_assert(ArraySize <= Size && ArraySize > 0);

        for (size_t i = 0; i < ArraySize; i++)
            this->entries[i] = data[i];

        if constexpr (ArraySize < Size)
        {
            for (size_t i = ArraySize; i < Size; i++)
                this->entries[i] = { Key(), Value() };
        }
    }

  public:
    using Key             = std::remove_cvref_t<K>;
    using Value           = std::remove_cvref_t<V>;
    using KeyComparator   = std::remove_cvref_t<KC>;
    using ValueComparator = std::remove_cvref_t<VC>;

    using Entry = std::pair<Key, Value>;

    BidirectionalMap() = delete;

    template<size_t ArraySize>
    constexpr BidirectionalMap(const std::pair<K, V> (&inEntries)[ArraySize], KC kc = KC {},
                               VC vc = VC {}) :
        entries(),
        populated(ArraySize),
        kc(kc),
        vc(vc)
    {
        populate<ArraySize>(inEntries);
    }

    template<size_t ArraySize>
    constexpr BidirectionalMap(const std::array<std::pair<K, V>, ArraySize>(&inEntries),
                               KC kc = KC {}, VC vc = VC {}) :
        entries(),
        populated(ArraySize),
        kc(kc),
        vc(vc)
    {
        populate<ArraySize>(inEntries.data());
    }

    constexpr BidirectionalMap(const std::pair<K, V> (&inEntries)[Size], KC kc = KC {},
                               VC vc = VC {}) :
        entries(),
        populated(Size),
        kc(kc),
        vc(vc)
    {
        populate<Size>(inEntries);
    }

    constexpr BidirectionalMap(const std::array<std::pair<K, V>, Size>(&inEntries), KC kc = KC {},
                               VC vc = VC {}) :
        entries(),
        populated(Size),
        kc(kc),
        vc(vc)
    {
        populate<Size>(inEntries.data());
    }

    constexpr bool Find(const Key& search, Value& out) const
    {
        for (size_t i = 0; i < this->populated; ++i)
        {
            if (kc(this->entries[i].first, search))
            {
                out = this->entries[i].second;
                return true;
            }
        }

        return false;
    }

    constexpr bool ReverseFind(const Value& search, Key& out) const
    {
        for (size_t i = 0; i < this->populated; ++i)
        {
            if (vc(this->entries[i].second, search))
            {
                out = this->entries[i].first;
                return true;
            }
        }

        return false;
    }

    /* Can only be used on String-mapped Keys */
    constexpr std::vector<const char*> GetNames() const
    {
        std::vector<const char*> strings;
        strings.reserve(this->populated);

        for (size_t i = 0; i < this->populated; i++)
        {
            if (this->entries[i].first != nullptr)
                strings.emplace_back(this->entries[i].first);
        }

        return strings;
    }

    constexpr std::pair<const Entry*, size_t> GetEntries() const
    {
        return { entries.data(), this->populated };
    }

  private:
    std::array<Entry, Size> entries;
    const size_t populated;

    [[no_unique_address]] KeyComparator kc;
    [[no_unique_address]] ValueComparator vc;
};

template<>
class BidirectionalMap<>
{
  public:
    // Used to compare C strings; operator== doesn't work properly for those
    struct cstringcomp
    {
        constexpr bool operator()(const char* a, const char* b) const
        {
            size_t idx = 0;
            while (a[idx] != '\0' && b[idx] != '\0')
            {
                if (a[idx] != b[idx])
                {
                    return false;
                }
                idx++;
            }

            if ((a[idx] == '\0') != (b[idx] == '\0'))
            {
                return false;
            }

            return true;
        }

        constexpr bool operator()(const char* a, std::string_view b) const
        {
            return a == b;
        }

        constexpr bool operator()(std::string_view b, const char* a) const
        {
            return a == b;
        }
    };

  private:
    template<typename A, typename B, typename... Args>
    struct CheckArgs
    {
      private:
        using CheckArgs2 = CheckArgs<Args...>;

        static constexpr bool KeyConvertible =
            std::is_convertible_v<std::remove_cvref_t<A>,
                                  std::remove_cvref_t<typename CheckArgs2::AType>> ||
            std::is_convertible_v<std::remove_cvref_t<typename CheckArgs2::AType>,
                                  std::remove_cvref_t<A>>;
        static constexpr bool ValueConvertible =
            std::is_convertible_v<std::remove_cvref_t<B>,
                                  std::remove_cvref_t<typename CheckArgs2::BType>> ||
            std::is_convertible_v<std::remove_cvref_t<typename CheckArgs2::BType>,
                                  std::remove_cvref_t<B>>;

      public:
        static constexpr bool value = KeyConvertible && ValueConvertible && CheckArgs2::value;

        using AType = std::conditional_t<
            sizeof...(Args) == 0, A,
            std::conditional_t<
                std::is_convertible_v<std::remove_cvref_t<typename CheckArgs2::AType>,
                                      std::remove_cvref_t<A>>,
                std::remove_cvref_t<A>, std::remove_cvref_t<typename CheckArgs2::AType>>>;
        using BType = std::conditional_t<
            sizeof...(Args) == 0, B,
            std::conditional_t<
                std::is_convertible_v<std::remove_cvref_t<typename CheckArgs2::BType>,
                                      std::remove_cvref_t<B>>,
                std::remove_cvref_t<B>, std::remove_cvref_t<typename CheckArgs2::BType>>>;
    };

    template<typename A, typename B>
    struct CheckArgs<A, B>
    {
        static constexpr bool value = true;
        using AType                 = std::remove_cvref_t<A>;
        using BType                 = std::remove_cvref_t<B>;
    };

    template<typename T>
    struct DefaultComparatorForType
    {
      private:
        using testtype = std::remove_cvref_t<T>;

      public:
        using value =
            std::conditional_t<std::is_same_v<testtype, const char*>, cstringcomp, std::equal_to<>>;
    };

    template<typename T>
    using defaultcomp_v = typename DefaultComparatorForType<T>::value;

  public:
    // clang-format off

    // Note: long name, but shouldn't often be used
    template<typename KeyComparator, typename ValueComparator, typename... Args>
        requires
            (sizeof...(Args) % 2 == 0) &&
            (sizeof...(Args) > 0) &&
            (CheckArgs<Args...>::value)
    static constexpr auto CreateWithComparators(KeyComparator kc, ValueComparator vc, Args... args)
    {
        using check = CheckArgs<Args...>;

        auto setArgs = []<typename K, typename V, size_t Size>(
                           std::array<std::pair<K, V>, Size>& addTo, Args... args) {
            auto setArgsRef = []<typename A, typename B, typename... InnerArgs>(
                auto& me, std::array<std::pair<K, V>, Size>& addTo, A key, B val, InnerArgs... args)
            {
                size_t index        = Size - (sizeof...(InnerArgs) + 2) / 2;
                addTo[index].first  = key;
                addTo[index].second = val;

                if constexpr (sizeof...(InnerArgs) != 0)
                {
                    me(me, addTo, std::forward<InnerArgs>(args)...);
                }
            };
            setArgsRef(setArgsRef, addTo, args...);
        };

        std::array<std::pair<typename check::AType, typename check::BType>, sizeof...(Args) / 2>
            entries {};

        setArgs(entries, std::forward<Args>(args)...);

        return BidirectionalMap<typename check::AType, typename check::BType, sizeof...(Args) / 2,
                                KeyComparator, ValueComparator> { entries, kc, vc };
    }

    // Note: long name, but shouldn't often be used
    template<typename KeyComparator = std::equal_to<>, typename... Args>
        requires
            (sizeof...(Args) % 2 == 0) &&
            (sizeof...(Args) > 0) &&
            (CheckArgs<Args...>::value)
    static constexpr auto CreateWithKeyComparator(KeyComparator kc = KeyComparator(), Args... args)
    {
        return CreateWithComparators(std::move(kc), defaultcomp_v<typename CheckArgs<Args...>::BType>(), args...);
    }

    // Note: long name, but shouldn't often be used
    template<typename ValueComparator = std::equal_to<>, typename... Args>
        requires
            (sizeof...(Args) % 2 == 0) &&
            (sizeof...(Args) > 0) &&
            (CheckArgs<Args...>::value)
    static constexpr auto CreateWithValueComparator(ValueComparator vc = ValueComparator(), Args... args)
    {
        return CreateWithComparators(defaultcomp_v<typename CheckArgs<Args...>::AType>(), std::move(vc), args...);
    }

    template<typename... Args>
        requires
            (sizeof...(Args) % 2 == 0) &&
            (sizeof...(Args) > 0) &&
            (CheckArgs<Args...>::value)
    static constexpr auto Create(Args... args)
    {
        return CreateWithComparators(defaultcomp_v<typename CheckArgs<Args...>::AType>(), defaultcomp_v<typename CheckArgs<Args...>::BType>(), args...);
    }

    // clang-format on
};
