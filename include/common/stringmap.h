#pragma once

#include <string>
#include <utility>
#include <vector>

#include "common/debug/logger.h"

template<typename T, size_t SIZE>
class StringMap
{
  public:
    struct Entry
    {
        const char* key;
        T value;
    };

    StringMap() = delete;

    template<size_t ARRAY_SIZE>
    consteval StringMap(const Entry (&inEntries)[ARRAY_SIZE]) : entries(), populated(ARRAY_SIZE)
    {
        static_assert(ARRAY_SIZE <= SIZE && ARRAY_SIZE > 0);

        for (size_t i = 0; i < ARRAY_SIZE; i++)
            this->entries[i] = inEntries[i];

        for (size_t i = ARRAY_SIZE; i < SIZE; i++)
            this->entries[i] = { nullptr, T(0) };
    }

    constexpr bool Find(const char* key, T& value) const
    {
        for (size_t i = 0; i < this->populated; ++i)
        {
            if (StringMap::StringEqual(this->entries[i].key, key))
            {
                value = this->entries[i].value;
                return true;
            }
        }

        return false;
    }

    constexpr bool Find(T key, const char*& string) const
    {
        for (size_t i = 0; i < this->populated; ++i)
        {
            if (this->entries[i].value == key)
            {
                string = this->entries[i].key;
                return true;
            }
        }

        return false;
    }

    constexpr std::vector<const char*> GetNames() const
    {
        std::vector<const char*> strings;
        strings.reserve(this->populated);

        for (size_t i = 0; i < this->populated; i++)
        {
            if (this->entries[i].key != nullptr)
                strings.emplace_back(this->entries[i].key);
        }

        return strings;
    }

    constexpr std::pair<const Entry*, size_t> GetEntries() const
    {
        return { entries, this->populated };
    }

  private:
    constexpr static bool StringEqual(const char* a, const char* b)
    {
        while (*a != 0 && *b != 0)
        {
            if (*a != *b)
                return false;

            ++a;
            ++b;
        }

        return (*a == 0 && *b == 0);
    };

    Entry entries[SIZE];
    const size_t populated;
};
