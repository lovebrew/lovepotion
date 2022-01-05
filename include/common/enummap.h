#pragma once

#include "common/exception.h"

namespace love
{
    template<typename T, typename U, size_t SIZE>
    class EnumMap
    {
      public:
        struct Entry
        {
            T t;
            U u;
        };

        EnumMap() = delete;

        template<size_t ARRAY_SIZE>
        consteval EnumMap(const Entry (&inEntries)[ARRAY_SIZE]) : entries(), populated(ARRAY_SIZE)
        {
            static_assert(ARRAY_SIZE <= SIZE && ARRAY_SIZE > 0);

            for (size_t i = 0; i < ARRAY_SIZE; i++)
                this->entries[i] = inEntries[i];

            for (size_t i = ARRAY_SIZE; i < SIZE; i++)
                this->entries[i] = { T(0), U(0) };
        }

        constexpr bool Find(T key, U& value) const
        {
            for (size_t i = 0; i < this->populated; ++i)
            {
                if (this->entries[i].t == key)
                {
                    value = this->entries[i].u;
                    return true;
                }
            }

            return false;
        }

        constexpr bool Find(U key, T& value) const
        {
            for (size_t i = 0; i < this->populated; ++i)
            {
                if (this->entries[i].u == key)
                {
                    value = this->entries[i].t;
                    return true;
                }
            }

            return false;
        }

      private:
        Entry entries[SIZE];
        const size_t populated;
    };
} // namespace love
