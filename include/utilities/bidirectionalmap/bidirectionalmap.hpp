#pragma once

#include <array>
#include <functional>
#include <ranges>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

namespace BidirectionalMapInternals
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
    template <typename ActualKeyType, typename ActualValueType,
        std::convertible_to<ActualKeyType> ArgKey, std::convertible_to<ActualValueType> ArgVal,
        typename... Args>
        requires (sizeof...(Args) % 2 == 0) && (sizeof...(Args) > 0) && ConvertibleArgs<ActualKeyType, ActualValueType, Args...>::value
    struct ConvertibleArgs<ActualKeyType, ActualValueType, ArgKey, ArgVal, Args...> : public std::true_type
    // clang-format on
    {
    };
}; // namespace BidirectionalMapInternals

// clang-format off
template<std::movable Key, std::movable Value, std::size_t Size,
         std::equivalence_relation<Key, Key> KeyComparator = std::equal_to<>, std::equivalence_relation<Value, Value> ValueComparator = std::equal_to<>>
    requires (Size > 0) &&
    std::same_as<Key, std::remove_cvref_t<Key>> &&
    std::same_as<Value, std::remove_cvref_t<Value>> &&
    std::same_as<KeyComparator, std::remove_cvref_t<KeyComparator>> &&
    std::same_as<ValueComparator, std::remove_cvref_t<ValueComparator>>
class BidirectionalMap
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

    template <std::ranges::contiguous_range PairRange, std::convertible_to<KeyComparator> KC,
        std::convertible_to<ValueComparator> VC, std::size_t... PairSeq>
    consteval BidirectionalMap(std::ranges::owning_view<PairRange>&& pairs, KC&& kc, VC&& vc,
        std::index_sequence<PairSeq...>)
        : entries{{std::forward<std::ranges::range_value_t<PairRange>>(
              *(pairs.begin() + PairSeq))...}},
          populated{std::min(std::ranges::size(pairs), Size)},
          kc{std::forward<KC>(kc)},
          vc{std::forward<VC>(vc)}
    {
    }

    template <std::ranges::contiguous_range PairRange, std::convertible_to<KeyComparator> KC,
        std::convertible_to<ValueComparator> VC, std::size_t RangeSize>
    consteval BidirectionalMap(std::ranges::owning_view<PairRange>&& pairs, KC&& kc, VC&& vc,
        std::integral_constant<std::size_t, RangeSize>)
        : BidirectionalMap(std::forward<decltype(pairs)>(pairs), std::forward<KC>(kc),
              std::forward<VC>(vc), std::make_index_sequence<RangeSize>{})
    {
    }

public:
    BidirectionalMap() = delete;

    // clang-format off
    template<std::convertible_to<Key> CurrentKey   = Key,
             std::convertible_to<Value> CurrentVal = Value,
             std::size_t ArraySize = Size,
             std::convertible_to<KeyComparator> KC = KeyComparator,
             std::convertible_to<ValueComparator> VC = ValueComparator>
    requires (ArraySize <= Size)
    consteval BidirectionalMap(std::pair<CurrentKey, CurrentVal> (&&inEntries)[ArraySize], KC&& kc = KC {}, VC&& vc = VC {}) :
        BidirectionalMap(std::ranges::owning_view(std::forward<decltype(inEntries)>(inEntries)),
                         std::forward<KC>(kc), std::forward<VC>(vc), std::integral_constant<std::size_t, ArraySize>{})
    {}

    template<std::convertible_to<Key> CurrentKey   = Key,
             std::convertible_to<Value> CurrentVal = Value,
             std::size_t ArraySize = Size,
             std::convertible_to<KeyComparator> KC = KeyComparator,
             std::convertible_to<ValueComparator> VC = ValueComparator>
    requires (ArraySize <= Size)
    consteval BidirectionalMap(std::array<std::pair<CurrentKey, CurrentVal>, ArraySize>&& inEntries, KC&& kc = KC {}, VC&& vc = VC {}) :
        BidirectionalMap(std::ranges::owning_view(std::forward<decltype(inEntries)>(inEntries)),
                         std::forward<KC>(kc), std::forward<VC>(vc), std::integral_constant<std::size_t, ArraySize>{})
    {}

    template<typename... Args, std::convertible_to<KeyComparator> KC, std::convertible_to<ValueComparator> VC>
    requires BidirectionalMapInternals::ConvertibleArgs<Key, Value, Args...>::value && (sizeof...(Args) / 2 <= Size) 
    consteval BidirectionalMap(KC&& kc, VC&& vc, Args&&... args) :
        BidirectionalMap(buildArgArray(std::forward<Args>(args)...), std::forward<KC>(kc), std::forward<VC>(vc))
    {}

    template<typename... Args>
    requires BidirectionalMapInternals::ConvertibleArgs<Key, Value, Args...>::value && (sizeof...(Args) / 2 <= Size)
    consteval BidirectionalMap(Args&&... args) :
        BidirectionalMap(BidirectionalMapInternals::defaultcomp_v<Key>(), BidirectionalMapInternals::defaultcomp_v<Value>(), std::forward<Args>(args)...)
    {}
    // clang-format on

    /*
    ** When mapped as T, V -- find value V from T
    */
    // clang-format off
    template <typename K = Key>
    requires std::equivalence_relation<KeyComparator, Key, K> ||
             std::equivalence_relation<KeyComparator, K, Key>
    constexpr std::optional<std::reference_wrapper<const Value>> Find(const K& search) const
    // clang-format on
    {
        for (std::size_t i = 0; i < this->populated; ++i)
        {
            if (compareKey(this->entries[i].first, search))
            {
                return std::ref(this->entries[i].second);
            }
        }

        return std::nullopt;
    }

    /*
    ** When mapped as T, V -- find value T from V
    */
    // clang-format off
    template <typename V = Value>
    requires std::equivalence_relation<ValueComparator, Value, V> ||
             std::equivalence_relation<ValueComparator, V, Value>
    constexpr std::optional<std::reference_wrapper<const Key>> ReverseFind(const V& search) const
    // clang-format on
    {
        for (std::size_t i = 0; i < this->populated; ++i)
        {
            if (compareValue(this->entries[i].second, search))
            {
                return std::ref(this->entries[i].first);
            }
        }

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

    template <typename V>
    requires std::equivalence_relation<ValueComparator, Value, V>
    constexpr auto compareValue(const Value& v1, V&& v2) const
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

    // clang-format off
    template <typename K>
    requires std::equivalence_relation<KeyComparator, Key, K>
    constexpr auto compareKey(const Key& v1, K&& v2) const
    // clang-format on
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
template<typename KC, typename VC, typename... Args, typename Folded = BidirectionalMapInternals::FoldArgs<Args...>>
requires Folded::value
BidirectionalMap(KC&&, VC&&, Args&&...) -> BidirectionalMap<typename Folded::AType,
                                                            typename Folded::BType,
                                                            sizeof...(Args) / 2,
                                                            std::remove_cvref_t<KC>,
                                                            std::remove_cvref_t<VC>>;

template<typename... Args, typename Folded = BidirectionalMapInternals::FoldArgs<Args...>>
requires Folded::value
BidirectionalMap(Args&&...) -> BidirectionalMap<typename Folded::AType,
                                                typename Folded::BType,
                                                sizeof...(Args) / 2,
                                                typename Folded::KCType,
                                                typename Folded::VCType>;

template<typename K, typename V, typename KC, typename VC, std::size_t ArraySize>
BidirectionalMap(std::pair<K, V> (&&)[ArraySize], KC&&, VC&&) -> BidirectionalMap<std::remove_cvref_t<K>,
                                                                                  std::remove_cvref_t<V>,
                                                                                  ArraySize,
                                                                                  std::remove_cvref_t<KC>,
                                                                                  std::remove_cvref_t<VC>>;

template<typename K, typename V, std::size_t ArraySize>
BidirectionalMap(std::pair<K, V> (&&)[ArraySize]) -> BidirectionalMap<std::remove_cvref_t<K>,
                                                                      std::remove_cvref_t<V>,
                                                                      ArraySize,
                                                                      BidirectionalMapInternals::defaultcomp_v<std::remove_cvref_t<K>>,
                                                                      BidirectionalMapInternals::defaultcomp_v<std::remove_cvref_t<V>>>;

template<typename K, typename V, typename KC, typename VC, std::size_t ArraySize>
BidirectionalMap(std::array<std::pair<K, V>, ArraySize>&&, KC&&, VC&&) -> BidirectionalMap<std::remove_cvref_t<K>,
                                                                                           std::remove_cvref_t<V>,
                                                                                           ArraySize,
                                                                                           std::remove_cvref_t<KC>,
                                                                                           std::remove_cvref_t<VC>>;

template<typename K, typename V, std::size_t ArraySize>
BidirectionalMap(std::array<std::pair<K, V>, ArraySize>&&) -> BidirectionalMap<std::remove_cvref_t<K>,
                                                                               std::remove_cvref_t<V>,
                                                                               ArraySize,
                                                                               BidirectionalMapInternals::defaultcomp_v<std::remove_cvref_t<K>>,
                                                                               BidirectionalMapInternals::defaultcomp_v<std::remove_cvref_t<V>>>;
// clang-format on
