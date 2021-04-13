#pragma once

#include "common/exception.h"
#include "common/lmath.h"
#include "objects/object.h"

namespace love
{
    class RandomGenerator : public Object
    {
      public:
        static love::Type type;

        union Seed
        {
            u64 b64;

            struct
            {
                u32 low;
                u32 high;
            } b32;
        };

        RandomGenerator();

        virtual ~RandomGenerator() {};

        u64 UniformRandom();

        inline double Random()
        {
            u64 random = UniformRandom();
            union
            {
                u64 i;
                double d;
            } u;

            u.i = ((0x3FFULL) << 52) | (random >> 12);

            return u.d - 1.0;
        }

        inline double Random(double max)
        {
            return Random() * max;
        }

        inline double Random(double min, double max)
        {
            return Random() * (max - min) + min;
        }

        double RandomNormal(double stddev);

        void SetSeed(Seed seed);

        Seed GetSeed() const;

        void SetState(const std::string& state);

        std::string GetState() const;

      private:
        Seed seed;
        Seed state;

        double lastRandomNormal;
    };
} // namespace love
