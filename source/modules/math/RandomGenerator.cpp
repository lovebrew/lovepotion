#include "modules/math/RandomGenerator.hpp"

#include <format>

#include <cmath>
#include <cstdlib>

namespace love
{

    static uint64_t wangHash64(uint64_t key)
    {
        key = (~key) + (key << 21); // key = (key << 21) - key - 1;
        key = key ^ (key >> 24);
        key = (key + (key << 3)) + (key << 8); // key * 265
        key = key ^ (key >> 14);
        key = (key + (key << 2)) + (key << 4); // key * 21
        key = key ^ (key >> 28);
        key = key + (key << 31);

        return key;
    }

    Type RandomGenerator::type("RandomGenerator", &Object::type);

    RandomGenerator::RandomGenerator() : lastRandomNormal(std::numeric_limits<double>::infinity())
    {
        Seed newSeed {};
        newSeed.b32.low  = RandomGenerator::DEFAULT_LOW;
        newSeed.b32.high = RandomGenerator::DEFAULT_HIGH;

        this->setSeed(newSeed);
    }

    uint64_t RandomGenerator::rand()
    {
        this->state.b64 ^= (this->state.b64 >> 12);
        this->state.b64 ^= (this->state.b64 << 25);
        this->state.b64 ^= (this->state.b64 >> 27);

        return this->state.b64 * RandomGenerator::MULTIPLIER;
    }

    double RandomGenerator::randomNormal(double stddev)
    {
        if (this->lastRandomNormal != std::numeric_limits<double>::infinity())
        {
            double last            = this->lastRandomNormal;
            this->lastRandomNormal = std::numeric_limits<double>::infinity();

            return last * stddev;
        }

        double root = std::sqrt(-2.0 * std::log(1.0 - this->random()));
        double phi  = 2.0 * LOVE_M_PI * (1.0 - this->random());

        this->lastRandomNormal = root * std::cos(phi);

        return root * std::sin(phi) * stddev;
    }

    void RandomGenerator::setSeed(RandomGenerator::Seed newSeed)
    {
        this->seed = newSeed;

        do
        {
            newSeed.b64 = wangHash64(newSeed.b64);
        } while (newSeed.b64 == 0);

        this->state            = newSeed;
        this->lastRandomNormal = std::numeric_limits<double>::infinity();
    }

    RandomGenerator::Seed RandomGenerator::getSeed() const
    {
        return this->seed;
    }

    void RandomGenerator::setState(const std::string& stateString)
    {
        if (stateString.find("0x") != 0 || stateString.size() < 3)
            throw love::Exception("Invalid random state: {:s}", stateString);

        Seed newState {};
        char* end = nullptr;

        newState.b64 = std::strtoull(stateString.c_str(), &end, 16);

        if (end != nullptr && *end != 0)
            throw love::Exception("Invalid random state: {:s}", stateString);

        this->state            = newState;
        this->lastRandomNormal = std::numeric_limits<double>::infinity();
    }

    std::string RandomGenerator::getState() const
    {
        return std::format("0x{:016x}", this->state.b64);
    }
} // namespace love
