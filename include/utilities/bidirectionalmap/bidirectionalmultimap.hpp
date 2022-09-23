#pragma once

#include "smallvector.hpp"

#include <array>
#include <functional>
#include <string_view>
#include <type_traits>
#include <utility>

// Note: both Key and Value must have a default constructor
// Note: KeyComparator and ValueComparator must act similarly to std::equal_to
template<typename K = void, typename V = void, size_t Size = 0, typename KC = std::equal_to<>,
         typename VC = std::equal_to<>>
class BidirectionalMultiMap
{
    friend class BidirectionalMultiMap<>;

    static_assert(Size > 0);
    static_assert(!std::is_same_v<K, void>);
    static_assert(!std::is_same_v<V, void>);

  public:
    using Key             = std::remove_cvref_t<K>;
    using Value           = std::remove_cvref_t<V>;
    using KeyComparator   = std::remove_cvref_t<KC>;
    using ValueComparator = std::remove_cvref_t<VC>;

    using Entry = std::pair<Key, Value>;

  private:
    template<size_t ArraySize>
    consteval static void populate(std::array<Entry, Size>& entries, const std::pair<K, V>* data)
    {
        static_assert(ArraySize <= Size && ArraySize > 0);

        for (size_t i = 0; i < ArraySize; i++)
            entries[i] = data[i];

        if constexpr (ArraySize < Size)
        {
            for (size_t i = ArraySize; i < Size; i++)
                entries[i] = { Key(), Value() };
        }
    }

  public:
    BidirectionalMultiMap() = delete;

    template<size_t ArraySize>
    consteval BidirectionalMultiMap(const std::pair<K, V> (&inEntries)[ArraySize],
                                    const std::pair<K, V> (&inRevEntries)[ArraySize], KC kc = KC {},
                                    VC vc = VC {}) :
        entries(),
        revEntries(),
        populated(ArraySize),
        kc(kc),
        vc(vc)
    {
        populate<ArraySize>(entries, inEntries);
        populate<ArraySize>(revEntries, inRevEntries);
    }

    template<size_t ArraySize>
    consteval BidirectionalMultiMap(const std::array<std::pair<K, V>, ArraySize>(&inEntries),
                                    const std::array<std::pair<K, V>, ArraySize>(&inRevEntries),
                                    KC kc = KC {}, VC vc = VC {}) :
        entries(),
        revEntries(),
        populated(ArraySize),
        kc(kc),
        vc(vc)
    {
        populate<ArraySize>(entries, inEntries.data());
        populate<ArraySize>(revEntries, inRevEntries.data());
    }

    consteval BidirectionalMultiMap(const std::pair<K, V> (&inEntries)[Size],
                                    const std::pair<K, V> (&inRevEntries)[Size], KC kc = KC {},
                                    VC vc = VC {}) :
        entries(),
        revEntries(),
        populated(Size),
        kc(kc),
        vc(vc)
    {
        populate<Size>(entries, inEntries);
        populate<Size>(revEntries, inRevEntries);
    }

    consteval BidirectionalMultiMap(const std::array<std::pair<K, V>, Size>(&inEntries),
                                    const std::array<std::pair<K, V>, Size>(&inRevEntries),
                                    KC kc = KC {}, VC vc = VC {}) :
        entries(),
        revEntries(),
        populated(Size),
        kc(kc),
        vc(vc)
    {
        populate<Size>(entries, inEntries.data());
        populate<Size>(revEntries, inRevEntries.data());
    }

    /*
    ** When mapped as T, V -- find value V from T
    */
    constexpr SmallTrivialVector<Value, Size> Find(const Key& search) const
    {
        SmallTrivialVector<Value, Size> ret;

        auto first = std::find_if(this->entries.begin(), this->entries.end(),
                                  [&](const Entry& e) { return kc(e.first, search); });
        auto last =
            std::find_if(std::reverse_iterator(this->entries.begin() + populated),
                         this->entries.rend(), [&](const Entry& e) { return kc(e.first, search); })
                .base();

        while (first != last)
        {
            ret.push_back(first->second);
            ++first;
        }

        return ret;
    }

    /*
    ** When mapped as T, V -- find value T from V
    */
    constexpr SmallTrivialVector<Key, Size> ReverseFind(const Value& search) const
    {
        SmallTrivialVector<Value, Size> ret;

        auto first = std::find_if(this->revEntries.begin(), this->revEntries.begin() + populated,
                                  [&](const Entry& e) { return kc(e.second, search); });
        auto last  = std::find_if(std::reverse_iterator(this->revEntries.begin() + populated),
                                  this->revEntries.rend(),
                                  [&](const Entry& e) { return kc(e.second, search); })
                        .base();

        while (first != last)
        {
            ret.push_back(first->second);
            ++first;
        }

        return ret;
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
    std::array<Entry, Size> revEntries;
    const size_t populated;

    [[no_unique_address]] KeyComparator kc;
    [[no_unique_address]] ValueComparator vc;
};

template<>
class BidirectionalMultiMap<>
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
    static consteval auto CreateWithComparators(KeyComparator kc, ValueComparator vc, Args... args)
    {
        using check = CheckArgs<Args...>;
        using AType = typename check::AType;
        using BType = typename check::BType;
        using PairType = std::pair<AType, BType>;
        constexpr size_t Size = sizeof...(Args) / 2;

        auto setArgs = [](std::array<PairType, Size>& addTo, Args... args) {
            auto setArgsRef = []<typename... InnerArgs>(
                auto& me, std::array<PairType, Size>& addTo, AType key, BType val, InnerArgs... args)
            {
                size_t index        = addTo.size() - (sizeof...(InnerArgs) + 2) / 2;
                addTo[index].first  = key;
                addTo[index].second = val;

                if constexpr (sizeof...(InnerArgs) != 0)
                {
                    me(me, addTo, std::forward<InnerArgs>(args)...);
                }
            };
            setArgsRef(setArgsRef, addTo, args...);
        };

        std::array<PairType, Size> entries {};

        setArgs(entries, std::forward<Args>(args)...);

        std::array<PairType, Size> reverseEntries = entries;

        // Chunk sort? I guess that's what I'll call this lol
        // Gets all of a single key into a contiguous block
        auto chunksort = []<typename Comparator, typename Accessor>(std::array<PairType, Size>& sortMe, Comparator c, Accessor a){
            size_t CurrentCheck = 0;

            while (CurrentCheck < sortMe.size() - 1)
            {
                const auto& current = sortMe[CurrentCheck];

                size_t swapWith = CurrentCheck + 1;
                while (swapWith < sortMe.size() - 1 && c(std::invoke(a, current), std::invoke(a, sortMe[swapWith])))
                {
                    swapWith++;
                }

                if (swapWith == sortMe.size() - 1)
                {
                    break;
                }

                for (size_t findMatchesCurrent = swapWith + 1; findMatchesCurrent < sortMe.size(); findMatchesCurrent++)
                {
                    if (c(std::invoke(a, current), std::invoke(a, sortMe[findMatchesCurrent])))
                    {
                        std::swap(sortMe[swapWith], sortMe[findMatchesCurrent]);
                        break;
                    }
                }

                if (c(std::invoke(a, current), std::invoke(a, sortMe[swapWith])))
                {
                    CurrentCheck = swapWith;
                }
                else
                {
                    CurrentCheck++;
                }
            }
        };

        chunksort(entries, kc, &PairType::first);
        chunksort(reverseEntries, vc, &PairType::second);



        return BidirectionalMultiMap<AType, BType, sizeof...(Args) / 2,
                                KeyComparator, ValueComparator> { entries, reverseEntries, kc, vc };
    }

    // Note: long name, but shouldn't often be used
    template<typename KeyComparator = std::equal_to<>, typename... Args>
        requires
            (sizeof...(Args) % 2 == 0) &&
            (sizeof...(Args) > 0) &&
            (CheckArgs<Args...>::value)
    static consteval auto CreateWithKeyComparator(KeyComparator kc = KeyComparator(), Args... args)
    {
        return CreateWithComparators(std::move(kc), defaultcomp_v<typename CheckArgs<Args...>::BType>(), args...);
    }

    // Note: long name, but shouldn't often be used
    template<typename ValueComparator = std::equal_to<>, typename... Args>
        requires
            (sizeof...(Args) % 2 == 0) &&
            (sizeof...(Args) > 0) &&
            (CheckArgs<Args...>::value)
    static consteval auto CreateWithValueComparator(ValueComparator vc = ValueComparator(), Args... args)
    {
        return CreateWithComparators(defaultcomp_v<typename CheckArgs<Args...>::AType>(), std::move(vc), args...);
    }

    template<typename... Args>
        requires
            (sizeof...(Args) % 2 == 0) &&
            (sizeof...(Args) > 0) &&
            (CheckArgs<Args...>::value)
    static consteval auto Create(Args... args)
    {
        return CreateWithComparators(defaultcomp_v<typename CheckArgs<Args...>::AType>(), defaultcomp_v<typename CheckArgs<Args...>::BType>(), args...);
    }

    // clang-format on
};
