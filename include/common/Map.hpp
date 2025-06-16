#pragma once

#include <algorithm>

#include <array>
#include <vector>

#include <ranges>
using std::ranges::views::keys;
using std::ranges::views::values;

#include <utility>

#include <string>
#include <string_view>

#include <format>
#include <numeric>

#include <type_traits>

template<typename K, typename V, std::size_t N>
requires(N > 0)
class MapT
{
    using Entry = std::pair<K, V>;

  private:
    std::array<Entry, N> items;

    template<typename Search, typename Projection>
    constexpr auto find(const Search& search, Projection projection) const
    {
        return std::ranges::find(this->items, search, projection);
    }

  public:
    using KeyType   = std::conditional_t<std::is_same_v<K, std::string_view>, std::string, K>;
    using ValueType = V;

    MapT() = delete;

    MapT(MapT&&) = delete;

    MapT(const MapT&) = delete;

    /**
     * @param items The items to initialize the map with
     * @brief Construct a map with the given items
     */
    constexpr MapT(const std::array<Entry, N>& items) : items(items)
    {}

    constexpr MapT(const std::array<std::pair<const char*, V>, N>& items)
    {
        std::ranges::transform(items, this->items.begin(), [](const auto& item) {
            return std::pair(std::string_view(item.first), item.second);
        });
    }

    constexpr size_t size() const
    {
        return this->items.size();
    }

    /**
     * @brief Get the value associated with the key
     * @param key The key to search for
     * @param value The output value
     * @return bool true if the key exists, false otherwise
     */
    constexpr bool getValue(const K& key, V& value) const
    {
        auto it = this->find(key, &Entry::first);

        if (it != this->items.end())
        {
            value = it->second;
            return true;
        }

        return false;
    }

    /**
     * @param value The value to search for
     * @param key The output key
     * @brief Get the key associated with the value
     * @return bool true if the value exists, false otherwise
     */
    constexpr bool getKey(const V& value, K& key) const
    {
        auto it = this->find(value, &Entry::second);

        if (it != this->items.end())
        {
            key = it->first;
            return true;
        }

        return false;
    }

    /**
     * Get the expected values of the map
     * @param type The type of the map (e.g. "Color")
     * @param value The value that was not found
     * @return std::string containing the expected values (comma separated)
     */
    constexpr std::string expected(std::string_view type, std::string_view value) const
    {
        auto concat = [](std::string&& a, const Entry& b) -> std::string& {
            return a.empty() ? a.append(b.first) : a.append(", ").append(b.first);
        };

        auto v = std::accumulate(this->items.begin(), this->items.end(), std::string {}, concat);
        return std::format("Invalid {} '{}': expected one of {}.", type, value, v);
    }
};

template<typename K, typename V, std::size_t N>
MapT(std::pair<K, V> (&&)[N]) -> MapT<std::remove_cvref_t<K>, std::remove_cvref_t<V>, N>;

template<typename K, typename V, std::size_t N>
MapT(std::array<std::pair<K, V>, N>&&) -> MapT<std::remove_cvref_t<K>, std::remove_cvref_t<V>, N>;

template<typename V, std::size_t N>
using StringMap = MapT<std::string_view, V, N>;

// clang-format off
#define STRINGMAP_DECLARE(name, type, ...)                                                   \
    static constexpr StringMap name {                                                        \
        std::array<std::pair<std::string_view, type>,                                        \
        (std::initializer_list<std::pair<std::string_view, type>> { __VA_ARGS__ }).size()> { \
            { __VA_ARGS__ }                                                                  \
        }                                                                                    \
    };                                                                                       \
    static inline bool getConstant(const char* in, type& out)                                \
    {                                                                                        \
        return name.getValue(in, out);                                                       \
    }                                                                                        \
    static inline bool getConstant(type in, std::string_view& out)                           \
    {                                                                                        \
        return name.getKey(in, out);                                                         \
    }                                                                                        \
    static inline const char* getConstant(type in)                                           \
    {                                                                                        \
        std::string_view out {};                                                             \
        return name.getKey(in, out) ? out.data() : nullptr;                                  \
    } // clang-format on

template<typename T>
concept MapTIsEnumType = std::is_enum_v<T>;

template<MapTIsEnumType K, MapTIsEnumType V, std::size_t N>
using EnumMap = MapT<K, V, N>;

// clang-format off
#define ENUMMAP_DECLARE(name, type_a, type_b, ...)                                   \
    static constexpr EnumMap name {                                                  \
        std::array<std::pair<type_a, type_b>,                                        \
        (std::initializer_list<std::pair<type_a, type_b>> { __VA_ARGS__ }).size()> { \
            { __VA_ARGS__ }                                                          \
        }                                                                            \
    };                                                                               \
    static inline bool getConstant(type_a in, type_b& out)                           \
    {                                                                                \
        return name.getValue(in, out);                                               \
    }                                                                                \
    static inline bool getConstant(type_b in, type_a& out)                           \
    {                                                                                \
        return name.getKey(in, out);                                                 \
    }
// clang-format on
