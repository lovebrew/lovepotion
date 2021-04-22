#pragma once

#include "common/exception.h"

namespace love
{
    template<typename T, typename U, size_t PEAK>
    class EnumMap
    {
      public:
        struct Entry
        {
            T t;
            U u;
        };

        EnumMap(const Entry* entries, size_t size)
        {
            size_t n = size / sizeof(Entry);

            for (size_t i = 0; i < n; ++i)
            {
                size_t e_t = (size_t)entries[i].t;
                size_t e_u = (size_t)entries[i].u;

                if (e_t < PEAK)
                {
                    values_u[e_t].v   = e_u;
                    values_u[e_t].set = true;
                }

                if (e_u < PEAK)
                {
                    values_t[e_u].v   = e_t;
                    values_t[e_u].set = true;
                }
            }
        }

        bool Find(T t, U& u)
        {
            if ((size_t)t < PEAK && values_u[(size_t)t].set)
            {
                u = (U)values_u[(size_t)t].v;
                return true;
            }

            return false;
        }

        bool Find(U u, T& t)
        {
            if ((size_t)u < PEAK && values_t[(size_t)u].set)
            {
                t = (T)values_t[(size_t)u].v;
                return true;
            }

            return false;
        }

      private:
        struct Value
        {
            unsigned v;
            bool set;

            Value() : set(false)
            {}
        };

        Value values_t[PEAK];
        Value values_u[PEAK];
    };
} // namespace love