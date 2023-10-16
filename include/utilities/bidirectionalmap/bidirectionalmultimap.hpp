#pragma once

#include <array>
#include <functional>
#include <ranges>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

namespace BidirectionalMultiMapInternals
{
    // Used to compare C strings; operator== doesn't work properly for those
    struct cstringcomp
    {
        constexpr bool operator()(const char* a, const char* b) const
        {
            if (a == nullptr && b == nullptr)
            {
                return true;
            }
            else if (a == nullptr || b == nullptr)
            {
                return false;
            }
            return std::string_view(a) == std::string_view(b);
        }

        constexpr bool operator()(const char* a, std::string_view b) const { return a == b; }

        constexpr bool operator()(std::string_view b, const char* a) const { return a == b; }
    };

    template <typename T>
    struct DefaultComparatorForType
    {
    private:
        using testtype = std::remove_cvref_t<T>;

    public:
        using value =
            std::conditional_t<std::is_same_v<testtype, const char*>, cstringcomp, std::equal_to<>>;
    };

    template <typename T>
    using defaultcomp_v = typename DefaultComparatorForType<T>::value;

    template <typename A, typename B, typename... Args>
    struct FoldArgs
    {
    public:
        static constexpr bool value = true;

        using AType = std::remove_cvref_t<A>;
        using BType = std::remove_cvref_t<B>;

        using KCType = defaultcomp_v<AType>;
        using VCType = defaultcomp_v<BType>;
    };

    template <typename A, typename B, typename APrime, typename BPrime, typename... Args>
    struct FoldArgs<A, B, APrime, BPrime, Args...>
    {
    private:
        using SubFoldArgs = FoldArgs<A, B, APrime, BPrime>;
        using FoldArgs2 =
            FoldArgs<typename SubFoldArgs::AType, typename SubFoldArgs::BType, Args...>;

    public:
        static constexpr bool value = SubFoldArgs::value && FoldArgs2::value;

        using AType = typename FoldArgs2::AType;
        using BType = typename FoldArgs2::BType;

        using KCType = defaultcomp_v<AType>;
        using VCType = defaultcomp_v<BType>;
    };

    template <typename A, typename B, typename APrime, typename BPrime>
    struct FoldArgs<A, B, APrime, BPrime>
    {
    private:
        using UnCV_A = std::remove_cvref_t<A>;
        using UnCV_B = std::remove_cvref_t<B>;

        using UnCV_APrime = std::remove_cvref_t<APrime>;
        using UnCV_BPrime = std::remove_cvref_t<BPrime>;

        using Decay_A = std::decay_t<A>;
        using Decay_B = std::decay_t<B>;

        using Decay_APrime = std::decay_t<APrime>;
        using Decay_BPrime = std::decay_t<BPrime>;

        static constexpr bool KeyConvertible =
            std::is_convertible_v<A, UnCV_APrime> || std::is_convertible_v<APrime, UnCV_A> ||
            std::is_convertible_v<A, Decay_APrime> || std::is_convertible_v<APrime, Decay_A>;
        static constexpr bool ValueConvertible =
            std::is_convertible_v<B, UnCV_BPrime> || std::is_convertible_v<BPrime, UnCV_B> ||
            std::is_convertible_v<B, Decay_BPrime> || std::is_convertible_v<BPrime, Decay_B>;

    public:
        static constexpr bool value = KeyConvertible && ValueConvertible;

        // clang-format off
        using AType = std::conditional_t<
            std::is_convertible_v<APrime, UnCV_A>,
            UnCV_A,
            std::conditional_t<
                std::is_convertible_v<A, UnCV_APrime>,
                UnCV_APrime,
                std::conditional_t<std::is_convertible_v<APrime, Decay_A>,
                                   Decay_A,
                                   Decay_APrime
                >
            >
        >;
        using BType = std::conditional_t<
            std::is_convertible_v<BPrime, UnCV_B>,
            UnCV_B,
            std::conditional_t<
                std::is_convertible_v<B, UnCV_BPrime>,
                UnCV_BPrime,
                std::conditional_t<std::is_convertible_v<BPrime, Decay_B>,
                                   Decay_B,
                                   Decay_BPrime
                >
            >
        >;
        // clang-format on

        using KCType = defaultcomp_v<AType>;
        using VCType = defaultcomp_v<BType>;
    };

    template <typename ActualKeyType, typename ActualValueType, typename... Args>
    struct ConvertibleArgs : public std::false_type
    {
    };

    template <typename ActualKeyType, typename ActualValueType,
        std::convertible_to<ActualKeyType> ArgKey, std::convertible_to<ActualValueType> ArgVal>
    struct ConvertibleArgs<ActualKeyType, ActualValueType, ArgKey, ArgVal> : public std::true_type
    {
    };

    // clang-format off
    template <typename ActualKeyType, typename ActualValueType, std::convertible_to<ActualKeyType> ArgKey,
              std::convertible_to<ActualValueType> ArgVal, typename... Args>
        requires (sizeof...(Args) % 2 == 0) && (sizeof...(Args) > 0) && ConvertibleArgs<ActualKeyType, ActualValueType, Args...>::value
    struct ConvertibleArgs<ActualKeyType, ActualValueType, ArgKey, ArgVal, Args...> : public std::true_type
    // clang-format on
    {
    };
}; // namespace BidirectionalMultiMapInternals

// clang-format off
template<std::movable Key, std::movable Value, std::size_t Size,
         std::equivalence_relation<Key, Key> KeyComparator = std::equal_to<>, std::equivalence_relation<Value, Value> ValueComparator = std::equal_to<>>
    requires (Size > 0) &&
    std::same_as<Key, std::remove_cvref_t<Key>> &&
    std::same_as<Value, std::remove_cvref_t<Value>> &&
    std::same_as<KeyComparator, std::remove_cvref_t<KeyComparator>> &&
    std::same_as<ValueComparator, std::remove_cvref_t<ValueComparator>>
class BidirectionalMultiMap
// clang-format on
{
public:
    using key_type   = Key;
    using value_type = Value;

    using Entry = std::pair<Key, Value>;

private:
    static consteval void buildVectorHelper(
        std::vector<std::pair<Key, Value>>& fillMe, auto&& a, auto&& b, auto&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            fillMe.emplace_back(std::forward<decltype(a)>(a), std::forward<decltype(b)>(b));
        }
        else
        {
            fillMe.emplace_back(std::forward<decltype(a)>(a), std::forward<decltype(b)>(b));
            return buildVectorHelper(fillMe, std::forward<decltype(args)>(args)...);
        }
    }
    static consteval std::vector<std::pair<Key, Value>> buildArgVector(auto&&... args)
    {
        std::vector<std::pair<Key, Value>> ret;
        buildVectorHelper(ret, std::forward<decltype(args)>(args)...);
        return ret;
    }
    template <typename... Args>
    static consteval auto buildArgArray(Args&&... args)
    {
        auto buildArrayHelper =
            []<std::size_t... Indices>(auto&& v, std::index_sequence<Indices...>)
                ->std::array<std::pair<Key, Value>, Size>
        {
            return {{std::move(v[Indices])...}};
        };
        return buildArrayHelper(
            buildArgVector(std::forward<Args>(args)...), std::make_index_sequence<Size>{});
    }

    template <std::size_t ArraySize>
    static consteval auto buildRefArray(const std::array<Entry, ArraySize>& sortMe,
        std::size_t populated, auto&& comparator, auto&& accessor)
    {
        std::size_t newIndex = 0;

        std::array<std::size_t, ArraySize> sorted{};
        std::array<bool, ArraySize> alreadyAdded{};

        for (std::size_t i = 0; i < populated; i++)
        {
            if (!alreadyAdded[i])
            {
                sorted[newIndex++] = i;
                alreadyAdded[i]    = true;
                for (std::size_t j = i + 1; j < populated; j++)
                {
                    if (!alreadyAdded[j] &&
                        std::invoke(comparator, std::invoke(accessor, sortMe[j]),
                            std::invoke(accessor, sortMe[i])))
                    {
                        alreadyAdded[j]    = true;
                        sorted[newIndex++] = j;
                    }
                }
            }
        }

        for (std::size_t i = populated; i < ArraySize; i++)
        {
            sorted[i] = i;
        }

        return sorted;
    }

    template <std::ranges::contiguous_range PairRange, std::convertible_to<KeyComparator> KC,
        std::convertible_to<ValueComparator> VC, std::size_t... PairSeq>
    consteval BidirectionalMultiMap(std::ranges::owning_view<PairRange>&& pairs, KC&& kc, VC&& vc,
        std::index_sequence<PairSeq...>)
        : entries{{std::forward<std::ranges::range_value_t<PairRange>>(
              *(pairs.begin() + PairSeq))...}},
          populated{std::min(std::ranges::size(pairs), Size)},
          kc{std::forward<KC>(kc)},
          vc{std::forward<VC>(vc)},
          forwardEntries{buildRefArray(entries, populated, this->kc, &Entry::first)},
          backwardEntries{buildRefArray(entries, populated, this->vc, &Entry::second)}
    {
    }

    template <std::ranges::contiguous_range PairRange, std::convertible_to<KeyComparator> KC,
        std::convertible_to<ValueComparator> VC, std::size_t RangeSize>
    consteval BidirectionalMultiMap(std::ranges::owning_view<PairRange>&& pairs, KC&& kc, VC&& vc,
        std::integral_constant<std::size_t, RangeSize>)
        : BidirectionalMultiMap(std::forward<decltype(pairs)>(pairs), std::forward<KC>(kc),
              std::forward<VC>(vc), std::make_index_sequence<RangeSize>{})
    {
    }

public:
    BidirectionalMultiMap() = delete;

    // clang-format off
    template<std::convertible_to<Key> CurrentKey   = Key,
             std::convertible_to<Value> CurrentVal = Value,
             std::size_t ArraySize = Size,
             std::convertible_to<KeyComparator> KC = KeyComparator,
             std::convertible_to<ValueComparator> VC = ValueComparator>
    requires (ArraySize <= Size)
    consteval BidirectionalMultiMap(std::pair<CurrentKey, CurrentVal> (&&inEntries)[ArraySize], KC&& kc = KC {}, VC&& vc = VC {}) :
        BidirectionalMultiMap(std::ranges::owning_view(std::forward<decltype(inEntries)>(inEntries)),
                         std::forward<KC>(kc), std::forward<VC>(vc), std::integral_constant<std::size_t, ArraySize>{})
    {}

    template<std::convertible_to<Key> CurrentKey   = Key,
             std::convertible_to<Value> CurrentVal = Value,
             std::size_t ArraySize = Size,
             std::convertible_to<KeyComparator> KC = KeyComparator,
             std::convertible_to<ValueComparator> VC = ValueComparator>
    requires (ArraySize <= Size)
    consteval BidirectionalMultiMap(std::array<std::pair<CurrentKey, CurrentVal>, ArraySize>&& inEntries, KC&& kc = KC {}, VC&& vc = VC {}) :
        BidirectionalMultiMap(std::ranges::owning_view(std::forward<decltype(inEntries)>(inEntries)),
                         std::forward<KC>(kc), std::forward<VC>(vc), std::integral_constant<std::size_t, ArraySize>{})
    {}

    template<typename... Args, std::convertible_to<KeyComparator> KC, std::convertible_to<ValueComparator> VC>
    requires BidirectionalMultiMapInternals::ConvertibleArgs<Key, Value, Args...>::value && (sizeof...(Args) / 2 <= Size) 
    consteval BidirectionalMultiMap(KC&& kc, VC&& vc, Args&&... args) :
        BidirectionalMultiMap(buildArgArray(std::forward<Args>(args)...), std::forward<KC>(kc), std::forward<VC>(vc))
    {}

    template<typename... Args>
    requires BidirectionalMultiMapInternals::ConvertibleArgs<Key, Value, Args...>::value && (sizeof...(Args) / 2 <= Size)
    consteval BidirectionalMultiMap(Args&&... args) :
        BidirectionalMultiMap(BidirectionalMultiMapInternals::defaultcomp_v<Key>(), BidirectionalMultiMapInternals::defaultcomp_v<Value>(), std::forward<Args>(args)...)
    {}
    // clang-format on

    /*
    ** When mapped as T, V -- find values V from T
    */
    template <typename K = Key>
        requires std::equivalence_relation<KeyComparator, Key, K> ||
        std::equivalence_relation<KeyComparator, K, Key> constexpr auto Find(const K& search) const
    {
        auto range = frange();
        auto first = std::find_if(std::ranges::begin(range), std::ranges::end(range),
            [&search, this](const auto& e) { return compareKey(e.first, search); });
        auto last =
            std::find_if(std::ranges::rbegin(range), std::ranges::rend(range), [&](const auto& e) {
                return compareKey(e.first, search);
            }).base();

        return frange() | std::views::values | std::views::drop(first - std::ranges::begin(range)) |
               std::views::take(last - first);
    }

    template <typename K = Key>
        requires std::equivalence_relation<KeyComparator, Key, K> ||
        std::equivalence_relation<KeyComparator, K,
            Key> constexpr std::optional<std::reference_wrapper<const Value>>
        FindFirst(const K& search) const
    {
        auto found = Find(search);
        if (std::ranges::begin(found) != std::ranges::end(found))
            return {std::ref(*std::ranges::begin(found))};

        return std::nullopt;
    }

    /*
    ** When mapped as T, V -- find first VectorSize values T from V
    */
    template <typename V = Value>
        requires std::equivalence_relation<ValueComparator, Value, V> ||
        std::equivalence_relation<ValueComparator, V, Value> constexpr auto ReverseFind(
            const V& search) const
    {
        auto range = brange();
        auto first = std::find_if(std::ranges::begin(range), std::ranges::end(range),
            [&search, this](const auto& e) { return compareValue(e.second, search); });
        auto last =
            std::find_if(std::ranges::rbegin(range), std::ranges::rend(range), [&](const auto& e) {
                return compareValue(e.second, search);
            }).base();

        return brange() | std::views::keys | std::views::drop(first - std::ranges::begin(range)) |
               std::views::take(last - first);
    }

    template <typename V = Value>
        requires std::equivalence_relation<ValueComparator, Value, V> ||
        std::equivalence_relation<ValueComparator, V,
            Value> constexpr std::optional<std::reference_wrapper<const Key>>
        ReverseFindFirst(const V& search) const
    {
        auto found = ReverseFind(search);
        if (std::ranges::begin(found) != std::ranges::end(found))
            return {std::ref(*std::ranges::begin(found))};

        return std::nullopt;
    }

    constexpr auto GetKeys() const
    {
        return std::ranges::ref_view(this->entries) | std::views::keys |
               std::views::take(this->populated);
    }

    /* Can only be used on String-mapped Keys */
    // clang-format off
    constexpr auto GetNames() const
        requires std::is_same_v<Key, const char*> || std::is_same_v<Key, char*> ||
                 std::is_same_v<Key, std::string_view>
    // clang-format on
    {
        return GetKeys();
    }

    constexpr auto GetValues() const
    {
        return std::ranges::ref_view(this->entries) | std::views::values |
               std::views::take(this->populated);
    }

    constexpr std::span<const Entry> GetEntries() const
    {
        return {entries.begin(), entries.begin() + populated};
    }

private:
    std::array<Entry, Size> entries;
    std::size_t populated;

    [[no_unique_address]] KeyComparator kc;
    [[no_unique_address]] ValueComparator vc;

    std::array<std::size_t, Size> forwardEntries;
    std::array<std::size_t, Size> backwardEntries;

    constexpr auto frange() const
    {
        return forwardEntries |
               std::views::transform([this](std::size_t i) -> const Entry& { return entries[i]; }) |
               std::views::take(populated);
    }

    constexpr auto brange() const
    {
        return backwardEntries |
               std::views::transform([this](std::size_t i) -> const Entry& { return entries[i]; }) |
               std::views::take(populated);
    }

    template <typename V>
    requires std::equivalence_relation<ValueComparator, Value, V> constexpr auto compareValue(
        const Value& v1, V&& v2) const
    {
        return std::invoke(vc, v1, std::forward<V>(v2));
    }

    // clang-format off
    template<typename V>
    requires std::equivalence_relation<ValueComparator, V, Value> &&
             (!std::equivalence_relation<ValueComparator, Value, V>)
    constexpr auto compareValue(const Value& v1, V&& v2) const
    // clang-format on
    {
        return std::invoke(vc, std::forward<V>(v2), v1);
    }

    template <typename K>
    requires std::equivalence_relation<KeyComparator, Key, K> constexpr auto compareKey(
        const Key& v1, K&& v2) const
    {
        return std::invoke(kc, v1, std::forward<K>(v2));
    }

    // clang-format off
    template<typename K>
    constexpr auto compareKey(const Key& v1, K&& v2) const
    requires std::equivalence_relation<KeyComparator, K, Key> &&
             (!std::equivalence_relation<KeyComparator, Key, K>)
    // clang-format on
    {
        return std::invoke(kc, std::forward<K>(v2), v1);
    }
};

// clang-format off
template<typename KC, typename VC, typename... Args, typename Folded = BidirectionalMultiMapInternals::FoldArgs<Args...>>
requires Folded::value
BidirectionalMultiMap(KC&&, VC&&, Args&&...) -> BidirectionalMultiMap<typename Folded::AType,
                                                            typename Folded::BType,
                                                            sizeof...(Args) / 2,
                                                            std::remove_cvref_t<KC>,
                                                            std::remove_cvref_t<VC>>;

template<typename... Args, typename Folded = BidirectionalMultiMapInternals::FoldArgs<Args...>>
requires Folded::value
BidirectionalMultiMap(Args&&...) -> BidirectionalMultiMap<typename Folded::AType,
                                                typename Folded::BType,
                                                sizeof...(Args) / 2,
                                                typename Folded::KCType,
                                                typename Folded::VCType>;

template<typename K, typename V, typename KC, typename VC, std::size_t ArraySize>
BidirectionalMultiMap(std::pair<K, V> (&&)[ArraySize], KC&&, VC&&) -> BidirectionalMultiMap<std::remove_cvref_t<K>,
                                                                                  std::remove_cvref_t<V>,
                                                                                  ArraySize,
                                                                                  std::remove_cvref_t<KC>,
                                                                                  std::remove_cvref_t<VC>>;

template<typename K, typename V, std::size_t ArraySize>
BidirectionalMultiMap(std::pair<K, V> (&&)[ArraySize]) -> BidirectionalMultiMap<std::remove_cvref_t<K>,
                                                                      std::remove_cvref_t<V>,
                                                                      ArraySize,
                                                                      BidirectionalMultiMapInternals::defaultcomp_v<std::remove_cvref_t<K>>,
                                                                      BidirectionalMultiMapInternals::defaultcomp_v<std::remove_cvref_t<V>>>;

template<typename K, typename V, typename KC, typename VC, std::size_t ArraySize>
BidirectionalMultiMap(std::array<std::pair<K, V>, ArraySize>&&, KC&&, VC&&) -> BidirectionalMultiMap<std::remove_cvref_t<K>,
                                                                                           std::remove_cvref_t<V>,
                                                                                           ArraySize,
                                                                                           std::remove_cvref_t<KC>,
                                                                                           std::remove_cvref_t<VC>>;

template<typename K, typename V, std::size_t ArraySize>
BidirectionalMultiMap(std::array<std::pair<K, V>, ArraySize>&&) -> BidirectionalMultiMap<std::remove_cvref_t<K>,
                                                                               std::remove_cvref_t<V>,
                                                                               ArraySize,
                                                                               BidirectionalMultiMapInternals::defaultcomp_v<std::remove_cvref_t<K>>,
                                                                               BidirectionalMultiMapInternals::defaultcomp_v<std::remove_cvref_t<V>>>;
// clang-format on
