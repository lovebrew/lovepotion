#pragma once

#include <array>
#include <ranges>
#include <utility>

#include <string_view>

template<typename Key, typename Value, size_t Size>
requires(Size > 0)
class Map
{
  private:
    using Entry = std::pair<Key, Value>;

  public:
    constexpr Map(const std::array<Entry, Size>& entries) : items(entries)
    {}

    constexpr bool getKey(const Value& value, Key& key) const
    {
        auto it = this->find(value, &Entry::second);
        if (it != this->items.end())
            key = it->first;

        return it != this->items.end();
    }

    constexpr bool getValue(const Key& key, Value& value) const
    {
        auto it = this->find(key, &Entry::first);
        if (it != this->items.end())
            value = it->second;

        return it != this->items.end();
    }

    std::string getNames() const
    requires(std::same_as<std::remove_cvref_t<Key>, std::string_view>)
    {
        const auto names = std::ranges::ref_view(this->items) | std::views::keys;
        std::string result {};

        for (auto name : names)
        {
            result.empty() ? result.append("'") : result.append(", '");
            result.append(name);
            result.append("'");
        }

        return result;
    }

  private:
    const std::array<Entry, Size> items;

    template<typename Search, typename Projection>
    constexpr auto find(const Search& search, Projection projection) const
    {
        return std::ranges::find(this->items, search, projection);
    }
};

template<typename Value, size_t Size>
requires(std::is_enum_v<Value>)
using StringMap = Map<std::string_view, Value, Size>;

template<typename Key, typename Value, size_t Size>
requires(std::is_enum_v<Key> && std::is_enum_v<Value>)
using EnumMap = Map<Key, Value, Size>;

// clang-format off
#define STRINGMAP_DECLARE(name, enum_t, ...)                                                                               \
    static inline constexpr auto name = StringMap { std::to_array<std::pair<std::string_view, enum_t>>({ __VA_ARGS__ }) }; \
    static inline constexpr bool getConstant(const char* key, enum_t& out)        { return name.getValue(key, out); }      \
    static inline constexpr bool getConstant(enum_t value, std::string_view& out) { return name.getKey(value, out); }      \

#define ENUMMAP_DECLARE(name, enum_t_a, enum_t_b, ...)                                                             \
    static inline constexpr auto name = EnumMap { std::to_array<std::pair<enum_t_a, enum_t_b>>({ __VA_ARGS__ }) }; \
    static inline constexpr bool getConstant(enum_t_a in, enum_t_b& out) { return name.getValue(in, out); }        \
    static inline constexpr bool getConstant(enum_t_a in, enum_t_a& out) { return name.getKey(in, out);   }
// clang-format on
