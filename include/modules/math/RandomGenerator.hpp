#pragma once

#include "common/Exception.hpp"
#include "common/Object.hpp"
#include "common/int.hpp"
#include "common/math.hpp"

#include <limits>
#include <string>

namespace love
{
    class RandomGenerator : public Object
    {
      public:
        static Type type;

        union Seed
        {
            uint64_t b64;
            struct
            {
#if defined(LOVE_BIG_ENDIAN)
                uint32_t high;
                uint32_t low;
#else
                uint32_t low;
                uint32_t high;
#endif
            } b32;
        };

        RandomGenerator();

        virtual ~RandomGenerator()
        {}

        uint64_t rand();

        inline double random()
        {
            uint64_t value = rand();

            union
            {
                uint64_t i;
                double d;
            } u;

            u.i = ((0x3FFULL) << 52) | (value >> 12);
            return u.d - 1.0;
        }

        inline double random(double max)
        {
            return random() * max;
        }

        inline double random(double min, double max)
        {
            return random() * (max - min) + min;
        }

        double randomNormal(double stddev);

        void setSeed(Seed seed);

        Seed getSeed() const;

        void setState(const std::string& state);

        std::string getState() const;

      private:
        static constexpr auto DEFAULT_LOW  = 0xCBBF7A44;
        static constexpr auto DEFAULT_HIGH = 0x0139408D;

        static constexpr auto MULTIPLIER = 2685821657736338717ULL;

        Seed seed;
        Seed state;
        double lastRandomNormal;
    };
} // namespace love
