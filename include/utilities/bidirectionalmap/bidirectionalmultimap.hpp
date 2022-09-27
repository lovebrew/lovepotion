#pragma once

#include "smallvector.hpp"
#include <array>
#include <functional>
#include <string_view>
#include <type_traits>
#include <utility>

template<typename K = void, typename V = void, std::size_t Size = 0, typename KC = std::equal_to<>,
         typename VC = std::equal_to<>>
class BidirectionalMultiMap;

template<>
class BidirectionalMultiMap<>
{
  public:
    // Used to compare C strings; operator== doesn't work properly for those
    struct cstringcomp
    {
        constexpr bool operator()(const char* a, const char* b) const
        {
            std::size_t idx = 0;
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

    // clang-format off
    template<typename A, typename B, typename... Args>
    struct CheckArgs
    // clang-format on
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

    template<typename... Args>
    static constexpr bool ValidArgs_v = (sizeof...(Args) % 2 == 0) && (sizeof...(Args) > 0) &&
                                        (CheckArgs<Args...>::value);

    template<typename KC, typename VC, typename... Args>
    struct ValidComparatorArgs_s
    {
        using Check = CheckArgs<Args...>;
        using AType = Check::AType;
        using BType = Check::BType;

        static constexpr bool value =
            (sizeof...(Args) % 2 == 0) && (sizeof...(Args) > 0) && Check::value && (requires {
                requires std::equivalence_relation<KC, AType, AType>;
                requires std::equivalence_relation<VC, BType, BType>;
            });
    };

    template<typename KC, typename VC>
    struct ValidComparatorArgs_s<KC, VC>
    {
        static constexpr bool value = false;
    };

    template<typename KC, typename VC, typename... Args>
    static constexpr bool ValidComparatorArgs_v = ValidComparatorArgs_s<KC, VC, Args...>::value;

  public:
    // Note: long name, but shouldn't often be used
    template<typename KeyComparator, typename ValueComparator, typename... Args>
    requires ValidComparatorArgs_v<KeyComparator, ValueComparator, Args...>
    static consteval auto CreateWithComparators(KeyComparator kc, ValueComparator vc, Args... args)
    {
        using check                = CheckArgs<Args...>;
        using AType                = typename check::AType;
        using BType                = typename check::BType;
        using PairType             = std::pair<AType, BType>;
        constexpr std::size_t Size = sizeof...(Args) / 2;

        auto setArgs = [](std::array<PairType, Size>& addTo, Args... args) {
            auto setArgsRef = []<typename... InnerArgs>(auto& me, std::array<PairType, Size>& addTo,
                                                        AType key, BType val, InnerArgs... args) {
                std::size_t index   = Size - (sizeof...(InnerArgs) + 2) / 2;
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

        return BidirectionalMultiMap<AType, BType, Size, KeyComparator, ValueComparator> { entries,
                                                                                           kc, vc };
    }

    // Note: long name, but shouldn't often be used
    template<typename KeyComparator = std::equal_to<>, typename... Args>
    requires ValidComparatorArgs_v<KeyComparator, defaultcomp_v<typename CheckArgs<Args...>::BType>,
                                   Args...>
    static consteval auto CreateWithKeyComparator(KeyComparator kc = KeyComparator(), Args... args)
    {
        return CreateWithComparators(std::move(kc),
                                     defaultcomp_v<typename CheckArgs<Args...>::BType>(), args...);
    }

    // Note: long name, but shouldn't often be used
    template<typename ValueComparator = std::equal_to<>, typename... Args>
    requires ValidComparatorArgs_v<defaultcomp_v<typename CheckArgs<Args...>::AType>,
                                   ValueComparator, Args...>
    static consteval auto CreateWithValueComparator(ValueComparator vc = ValueComparator(),
                                                    Args... args)
    {
        return CreateWithComparators(defaultcomp_v<typename CheckArgs<Args...>::AType>(),
                                     std::move(vc), args...);
    }

    template<typename... Args>
    requires ValidArgs_v<Args...>
    static consteval auto Create(Args... args)
    {
        return CreateWithComparators(defaultcomp_v<typename CheckArgs<Args...>::AType>(),
                                     defaultcomp_v<typename CheckArgs<Args...>::BType>(), args...);
    }
};

// clang-format off
template<std::default_initializable K, std::default_initializable V, std::size_t Size,
         std::equivalence_relation<K, K> KC, std::equivalence_relation<V, V> VC>
    requires (Size > 0)
class BidirectionalMultiMap<K, V, Size, KC, VC> : private BidirectionalMultiMap<>
// clang-format on
{
  public:
    using Key             = std::remove_cvref_t<K>;
    using Value           = std::remove_cvref_t<V>;
    using KeyComparator   = std::remove_cvref_t<KC>;
    using ValueComparator = std::remove_cvref_t<VC>;

    using Entry = std::pair<Key, Value>;

    using BidirectionalMultiMap<>::CreateWithComparators;
    using BidirectionalMultiMap<>::CreateWithKeyComparator;
    using BidirectionalMultiMap<>::CreateWithValueComparator;

  private:
    template<std::size_t ArraySize, typename Comparator, typename Accessor>
    consteval static void populate(std::array<Entry, Size>& entries, const std::pair<K, V>* data,
                                   Comparator&& c, Accessor&& a)
    {
        static_assert(ArraySize <= Size && ArraySize > 0);

        for (std::size_t i = 0; i < ArraySize; i++)
            entries[i] = data[i];

        if constexpr (ArraySize < Size)
        {
            for (std::size_t i = ArraySize; i < Size; i++)
                entries[i] = { Key(), Value() };
        }

        constexpr auto chunksort = [](std::array<Entry, Size>& sortMe, auto&& comparator,
                                      auto&& accessor) {
            std::size_t newIndex = 0;
            std::array<Entry, Size> sorted {};
            std::array<bool, Size> alreadyAdded {};

            for (std::size_t i = 0; i < ArraySize; i++)
            {
                if (!alreadyAdded[i])
                {
                    sorted[newIndex++] = sortMe[i];
                    alreadyAdded[i]    = true;
                    for (std::size_t j = i + 1; j < ArraySize; j++)
                    {
                        if (!alreadyAdded[j] &&
                            std::invoke(comparator, std::invoke(accessor, sortMe[j]),
                                        std::invoke(accessor, sortMe[i])))
                        {
                            alreadyAdded[j]    = true;
                            sorted[newIndex++] = sortMe[j];
                        }
                    }
                }
            }

            sortMe = sorted;
        };

        chunksort(entries, c, a);
    }

  public:
    BidirectionalMultiMap() = delete;

    template<std::size_t ArraySize = Size>
    consteval BidirectionalMultiMap(const std::pair<K, V> (&inEntries)[ArraySize], KC kc = KC {},
                                    VC vc = VC {}) :
        entries(),
        revEntries(),
        populated(ArraySize),
        kc(kc),
        vc(vc)
    {
        populate<ArraySize>(entries, inEntries, kc, &std::pair<K, V>::first);
        populate<ArraySize>(revEntries, inEntries, vc, &std::pair<K, V>::second);
    }

    template<std::size_t ArraySize = Size>
    consteval BidirectionalMultiMap(const std::array<std::pair<K, V>, ArraySize>(&inEntries),
                                    KC kc = KC {}, VC vc = VC {}) :
        entries(),
        revEntries(),
        populated(ArraySize),
        kc(kc),
        vc(vc)
    {
        populate<ArraySize>(entries, inEntries.data(), kc, &std::pair<K, V>::first);
        populate<ArraySize>(revEntries, inEntries.data(), vc, &std::pair<K, V>::second);
    }

    template<typename... Args>
    requires ValidComparatorArgs_v<KC, VC, Args...>
    consteval BidirectionalMultiMap(KC kc, VC vc, Args... args) :
        entries {},
        populated(0),
        kc(kc),
        vc(vc)
    {
        using check                   = CheckArgs<Args...>;
        using AType                   = typename check::AType;
        using BType                   = typename check::BType;
        using PairType                = std::pair<AType, BType>;
        constexpr std::size_t CurSize = sizeof...(Args) / 2;

        auto setArgs = [](std::array<PairType, CurSize>& addTo, Args... args) {
            auto setArgsRef = []<typename... InnerArgs>(auto& me,
                                                        std::array<PairType, CurSize>& addTo,
                                                        AType key, BType val, InnerArgs... args) {
                std::size_t index   = addTo.size() - (sizeof...(InnerArgs) + 2) / 2;
                addTo[index].first  = key;
                addTo[index].second = val;

                if constexpr (sizeof...(InnerArgs) != 0)
                {
                    me(me, addTo, std::forward<InnerArgs>(args)...);
                }
            };
            setArgsRef(setArgsRef, addTo, args...);
        };

        std::array<PairType, CurSize> newEntries {};

        setArgs(newEntries, std::forward<Args>(args)...);

        populate<CurSize>(entries, newEntries.data(), kc, &std::pair<K, V>::first);
        populate<CurSize>(revEntries, newEntries.data(), vc, &std::pair<K, V>::second);
    }

    template<typename... Args>
    requires ValidArgs_v<Args...>
    consteval BidirectionalMultiMap(Args... args) :
        BidirectionalMultiMap(defaultcomp_v<K>(), defaultcomp_v<V>(), args...)
    {}

    /*
    ** When mapped as T, V -- find first VectorSize values V from T
    */
    template<std::size_t VectorSize = Size>
    constexpr SmallTrivialVector<Value, VectorSize> Find(const Key& search) const
    {
        SmallTrivialVector<Value, VectorSize> ret;

        auto first = std::find_if(this->entries.begin(), this->entries.end(),
                                  [&](const Entry& e) { return kc(e.first, search); });
        auto last =
            std::find_if(std::reverse_iterator(this->entries.begin() + populated),
                         this->entries.rend(), [&](const Entry& e) { return kc(e.first, search); })
                .base();

        while (first != last && ret.size() < ret.capacity())
        {
            ret.push_back(first->second);
            ++first;
        }

        return ret;
    }

    constexpr bool FindFirst(const Key& search, Value& out) const
    {
        auto found = Find<1>(search);
        if (found.size() == 1)
        {
            out = found[0];
            return true;
        }
        return false;
    }

    /*
    ** When mapped as T, V -- find first VectorSize values T from V
    */
    template<std::size_t VectorSize = Size>
    constexpr SmallTrivialVector<Key, VectorSize> ReverseFind(const Value& search) const
    {
        SmallTrivialVector<Value, VectorSize> ret;

        auto first = std::find_if(this->revEntries.begin(), this->revEntries.begin() + populated,
                                  [&](const Entry& e) { return kc(e.second, search); });
        auto last  = std::find_if(std::reverse_iterator(this->revEntries.begin() + populated),
                                  this->revEntries.rend(),
                                  [&](const Entry& e) { return kc(e.second, search); })
                        .base();

        while (first != last && ret.size() < ret.capacity())
        {
            ret.push_back(first->first);
            ++first;
        }

        return ret;
    }

    constexpr bool ReverseFindFirst(const Value& search, Key& out) const
    {
        auto found = ReverseFind<1>(search);
        if (found.size() == 1)
        {
            out = found[0];
            return true;
        }
        return false;
    }

    constexpr SmallTrivialVector<Key, Size> GetKeys() const
    {
        return SmallTrivialVector<Key, Size>(std::views::keys(this->entries) |
                                             std::views::take(this->populated));
    }

    /* Can only be used on String-mapped Keys */
    constexpr SmallTrivialVector<Key, Size> GetNames() const
        requires(std::is_same_v<Key, const char*> || std::is_same_v<Key, char*> ||
                 std::is_same_v<Key, std::string_view>)
    {
        return GetKeys();
    }

    constexpr SmallTrivialVector<Value, Size> GetValues() const
    {
        return SmallTrivialVector<Value, Size>(std::views::values(this->entries) |
                                               std::views::take(this->populated));
    }

    constexpr std::pair<const Entry*, std::size_t> GetEntries() const
    {
        return { entries.data(), this->populated };
    }

  private:
    std::array<Entry, Size> entries;
    std::array<Entry, Size> revEntries;
    const std::size_t populated;

    [[no_unique_address]] KeyComparator kc;
    [[no_unique_address]] ValueComparator vc;
};

// clang-format off
template<typename KC, typename VC, typename... Args>
requires BidirectionalMultiMap<>::ValidComparatorArgs_v<KC, VC, Args...>
BidirectionalMultiMap(KC, VC, Args...) -> BidirectionalMultiMap<typename BidirectionalMultiMap<>::CheckArgs<Args...>::AType,
                                                                typename BidirectionalMultiMap<>::CheckArgs<Args...>::BType,
                                                                sizeof...(Args) / 2, KC, VC>;

template<typename... Args>
requires BidirectionalMultiMap<>::ValidArgs_v<Args...>
BidirectionalMultiMap(Args...) -> BidirectionalMultiMap<typename BidirectionalMultiMap<>::CheckArgs<Args...>::AType,
                                                        typename BidirectionalMultiMap<>::CheckArgs<Args...>::BType,
                                                        sizeof...(Args) / 2,
                                                        BidirectionalMultiMap<>::defaultcomp_v<typename BidirectionalMultiMap<>::CheckArgs<Args...>::AType>,
                                                        BidirectionalMultiMap<>::defaultcomp_v<typename BidirectionalMultiMap<>::CheckArgs<Args...>::BType>>;
// clang-format on
