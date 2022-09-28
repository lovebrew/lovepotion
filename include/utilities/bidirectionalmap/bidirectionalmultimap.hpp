#pragma once

#include <array>
#include <functional>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <utility>

template<typename K = void, typename V = void, std::size_t Size = 0, typename KC = std::equal_to<>,
         typename VC = std::equal_to<>>
class BidirectionalMultiMap;

template<>
class BidirectionalMultiMap<>
{
  protected:
    BidirectionalMultiMap() = default;

  public:
    // Used to compare C strings; operator== doesn't work properly for those
    struct cstringcomp
    {
        constexpr bool operator()(const char* a, const char* b) const
        {
            return std::string_view(a) == std::string_view(b);
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

    template<typename A, typename B, typename... Args>
    struct CheckArgs
    {
      private:
        using CheckArgs2 = CheckArgs<Args...>;

        static constexpr bool KeyConvertible =
            std::is_convertible_v<std::remove_cvref_t<A>,
                                  std::remove_cvref_t<typename CheckArgs2::AType>> ||
            std::is_convertible_v<std::remove_cvref_t<typename CheckArgs2::AType>,
                                  std::remove_cvref_t<A>> ||
            std::is_convertible_v<std::remove_cvref_t<A>,
                                  std::decay_t<typename CheckArgs2::AType>> ||
            std::is_convertible_v<std::remove_cvref_t<typename CheckArgs2::AType>, std::decay_t<A>>;
        static constexpr bool ValueConvertible =
            std::is_convertible_v<std::remove_cvref_t<B>,
                                  std::remove_cvref_t<typename CheckArgs2::BType>> ||
            std::is_convertible_v<std::remove_cvref_t<typename CheckArgs2::BType>,
                                  std::remove_cvref_t<B>> ||
            std::is_convertible_v<std::remove_cvref_t<B>,
                                  std::decay_t<typename CheckArgs2::BType>> ||
            std::is_convertible_v<std::decay_t<typename CheckArgs2::BType>, std::remove_cvref_t<B>>;

      public:
        static constexpr bool value = KeyConvertible && ValueConvertible && CheckArgs2::value;

        using AType = std::conditional_t<
            std::is_convertible_v<std::remove_cvref_t<typename CheckArgs2::AType>,
                                  std::remove_cvref_t<A>>,
            std::remove_cvref_t<A>,
            std::conditional_t<
                std::is_convertible_v<std::remove_cvref_t<A>,
                                      std::remove_cvref_t<typename CheckArgs2::AType>>,
                std::remove_cvref_t<typename CheckArgs2::AType>,
                std::conditional_t<
                    std::is_convertible_v<std::remove_cvref_t<typename CheckArgs2::AType>,
                                          std::decay_t<A>>,
                    std::decay_t<A>, std::decay_t<typename CheckArgs2::AType>>>>;
        using BType = std::conditional_t<
            std::is_convertible_v<std::remove_cvref_t<typename CheckArgs2::BType>,
                                  std::remove_cvref_t<B>>,
            std::remove_cvref_t<B>,
            std::conditional_t<
                std::is_convertible_v<std::remove_cvref_t<B>,
                                      std::remove_cvref_t<typename CheckArgs2::BType>>,
                std::remove_cvref_t<typename CheckArgs2::BType>,
                std::conditional_t<
                    std::is_convertible_v<std::remove_cvref_t<typename CheckArgs2::BType>,
                                          std::decay_t<B>>,
                    std::decay_t<B>, std::decay_t<typename CheckArgs2::BType>>>>;
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
    struct ValidArgs_s : public std::false_type
    {
    };

    // clang-format off
    template<typename... Args>
    requires (sizeof...(Args) % 2 == 0) && (sizeof...(Args) > 0)
    struct ValidArgs_s<Args...>
    // clang-format on
    {
        static constexpr bool value = CheckArgs<Args...>::value;
    };

    template<typename... Args>
    static constexpr bool ValidArgs_v = ValidArgs_s<Args...>::value;

    template<typename KC, typename VC, typename... Args>
    struct ValidComparatorArgs_s : public std::false_type
    {
    };

    // clang-format off
    template<typename KC, typename VC, typename... Args>
    requires (sizeof...(Args) % 2 == 0) && (sizeof...(Args) > 0)
    struct ValidComparatorArgs_s<KC, VC, Args...>
    // clang-format on
    {
        using Check = CheckArgs<Args...>;
        using AType = Check::AType;
        using BType = Check::BType;

        static constexpr bool value = Check::value && (requires {
                                          requires std::equivalence_relation<KC, AType, AType>;
                                          requires std::equivalence_relation<VC, BType, BType>;
                                      });
    };

    template<typename KC, typename VC, typename... Args>
    static constexpr bool ValidComparatorArgs_v = ValidComparatorArgs_s<KC, VC, Args...>::value;

  public:
    // Note: long name, but shouldn't often be used
    // clang-format off
    template<typename KeyComparator, typename ValueComparator, typename... Args>
    requires ValidComparatorArgs_v<KeyComparator, ValueComparator, Args...>
    static consteval auto CreateWithComparators(KeyComparator kc, ValueComparator vc, Args... args)
    // clang-format on
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
    // clang-format off
    template<typename KeyComparator = std::equal_to<>, typename... Args>
    requires ValidComparatorArgs_v<KeyComparator, defaultcomp_v<typename CheckArgs<Args...>::BType>, Args...>
    static consteval auto CreateWithKeyComparator(KeyComparator kc = KeyComparator(), Args... args)
    // clang-format on
    {
        return CreateWithComparators(std::move(kc),
                                     defaultcomp_v<typename CheckArgs<Args...>::BType>(), args...);
    }

    // Note: long name, but shouldn't often be used
    // clang-format off
    template<typename ValueComparator = std::equal_to<>, typename... Args>
    requires ValidComparatorArgs_v<defaultcomp_v<typename CheckArgs<Args...>::AType>, ValueComparator, Args...>
    static consteval auto CreateWithValueComparator(ValueComparator vc = ValueComparator(), Args... args)
    // clang-format on
    {
        return CreateWithComparators(defaultcomp_v<typename CheckArgs<Args...>::AType>(),
                                     std::move(vc), args...);
    }

    // clang-format off
    template<typename... Args>
    requires ValidArgs_v<Args...>
    static consteval auto Create(Args... args)
    // clang-format on
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

    // clang-format off
    template<typename... Args>
    requires ValidComparatorArgs_v<KC, VC, Args...>
    consteval BidirectionalMultiMap(KC kc, VC vc, Args... args) :
        entries {},
        populated(0),
        kc(kc),
        vc(vc)
    // clang-format on
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

    // clang-format off
    template<typename... Args>
    requires ValidArgs_v<Args...>
    consteval BidirectionalMultiMap(Args... args) :
        BidirectionalMultiMap(defaultcomp_v<K>(), defaultcomp_v<V>(), args...)
    // clang-format on
    {}

    /*
    ** When mapped as T, V -- find values V from T
    */
    constexpr decltype(std::views::take(
        std::views::drop(std::views::values(
                             std::ranges::ref_view(std::declval<const std::array<Entry, Size>&>())),
                         0),
        0))
    Find(const Key& search) const
    {
        auto first = std::find_if(this->entries.begin(), this->entries.end(),
                                  [&](const Entry& e) { return kc(e.first, search); });
        auto last =
            std::find_if(std::reverse_iterator(this->entries.begin() + populated),
                         this->entries.rend(), [&](const Entry& e) { return kc(e.first, search); })
                .base();

        return std::views::take(
            std::views::drop(std::views::values(std::ranges::ref_view(this->entries)),
                             first - this->entries.begin()),
            last - first);
    }

    constexpr std::optional<std::reference_wrapper<const Value>> FindFirst(const Key& search) const
    {
        auto found = Find(search);
        if (std::ranges::begin(found) != std::ranges::end(found))
            return { std::ref(*std::ranges::begin(found)) };

        return std::nullopt;
    }

    /*
    ** When mapped as T, V -- find first VectorSize values T from V
    */
    constexpr decltype(std::views::take(
        std::views::drop(
            std::views::keys(std::ranges::ref_view(std::declval<const std::array<Entry, Size>&>())),
            0),
        0))
    ReverseFind(const Value& search) const
    {
        auto first = std::find_if(this->revEntries.begin(), this->revEntries.begin() + populated,
                                  [&](const Entry& e) { return kc(e.second, search); });
        auto last  = std::find_if(std::reverse_iterator(this->revEntries.begin() + populated),
                                  this->revEntries.rend(),
                                  [&](const Entry& e) { return kc(e.second, search); })
                        .base();

        return std::views::take(
            std::views::drop(std::views::keys(std::ranges::ref_view(this->entries)),
                             first - this->entries.begin()),
            last - first);
    }

    constexpr std::optional<std::reference_wrapper<const Key>> ReverseFindFirst(
        const Value& search) const
    {
        auto found = ReverseFind(search);
        if (std::ranges::begin(found) != std::ranges::end(found))
            return { std::ref(*std::ranges::begin(found)) };

        return std::nullopt;
    }

    constexpr decltype(std::views::take(
        std::views::keys(std::ranges::ref_view(std::declval<const std::array<Entry, Size>&>())), 0))
    GetKeys() const
    {
        return std::views::take(std::views::keys(std::ranges::ref_view(entries)), this->populated);
    }

    /* Can only be used on String-mapped Keys */
    // clang-format off
    constexpr auto GetNames() const -> decltype(GetKeys())
        requires (std::is_same_v<Key, const char*> || std::is_same_v<Key, char*> ||
                  std::is_same_v<Key, std::string_view>)
    // clang-format on
    {
        return GetKeys();
    }

    constexpr decltype(std::views::take(
        std::views::values(std::ranges::ref_view(std::declval<const std::array<Entry, Size>&>())),
        0))
    GetValues() const
    {
        return std::views::take(std::views::values(std::ranges::ref_view(entries)),
                                this->populated);
    }

    constexpr std::span<Entry> GetEntries() const
    {
        return { entries.begin(), entries.begin() + populated };
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
requires BidirectionalMultiMap<>::ValidComparatorArgs_v<KC, VC, Args&&...>
BidirectionalMultiMap(KC&&, VC&&, Args&&...) -> BidirectionalMultiMap<typename BidirectionalMultiMap<>::CheckArgs<Args&&...>::AType,
                                                      typename BidirectionalMultiMap<>::CheckArgs<Args&&...>::BType,
                                                      sizeof...(Args) / 2, KC, VC>;

template<typename... Args>
requires BidirectionalMultiMap<>::ValidArgs_v<Args&&...>
BidirectionalMultiMap(Args&&...) -> BidirectionalMultiMap<typename BidirectionalMultiMap<>::CheckArgs<Args&&...>::AType,
                                              typename BidirectionalMultiMap<>::CheckArgs<Args&&...>::BType,
                                              sizeof...(Args) / 2,
                                              BidirectionalMultiMap<>::defaultcomp_v<typename BidirectionalMultiMap<>::CheckArgs<Args&&...>::AType>,
                                              BidirectionalMultiMap<>::defaultcomp_v<typename BidirectionalMultiMap<>::CheckArgs<Args&&...>::BType>>;
// clang-format on
