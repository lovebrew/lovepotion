#pragma once

#include <common/exception.hpp>
#include <common/math.hpp>
#include <common/object.hpp>

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

#if defined(__WIIU__)
            struct
            {
                uint32_t high;
                uint32_t low;
            } b32;
#else
            struct
            {
                uint32_t low;
                uint32_t high;
            } b32;
#endif
        };

        RandomGenerator();

        virtual ~RandomGenerator()
        {}

        uint64_t Rand();

        inline double Random()
        {
            uint64_t random = this->Rand();

            union
            {
                uint64_t i;
                double d;
            } u;

            u.i = ((0x3FFULL) << 52) | (random >> 12);

            return u.d - 1.0;
        }

        inline double Random(double max)
        {
            return this->Random() * max;
        }

        inline double Random(double min, double max)
        {
            return this->Random() * (max - min) + min;
        }

        double RandomNormal(double stdDev);

        void SetSeed(Seed seed);

        Seed GetSeed() const;

        void SetState(const std::string& stateString);

        std::string_view GetState() const;

      private:
        static constexpr auto RANDOM_STATE_MUL = 2685821657736338717ULL;

        Seed seed;
        Seed randomState;
        double lastRandomNormal;
    };
} // namespace love
