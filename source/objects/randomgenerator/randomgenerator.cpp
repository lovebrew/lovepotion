#include <objects/randomgenerator/randomgenerator.hpp>

#include <cmath>
#include <cstdlib>
#include <iomanip>

using namespace love;

Type RandomGenerator::type("RandomGenerator", &Object::type);

// Thomas Wang's 64-bit integer hashing function:
// https://web.archive.org/web/20110807030012/http://www.cris.com/%7ETtwang/tech/inthash.htm
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

RandomGenerator::RandomGenerator() : lastRandomNormal(std::numeric_limits<double>::infinity())
{
    Seed newSeed {};
    newSeed.b32.low  = 0xCBBF7A44;
    newSeed.b32.high = 0x0139408D;

    this->SetSeed(newSeed);
}

uint64_t RandomGenerator::Rand()
{
    this->randomState.b64 ^= (this->randomState.b64 >> 12);
    this->randomState.b64 ^= (this->randomState.b64 << 25);
    this->randomState.b64 ^= (this->randomState.b64 >> 27);

    return this->randomState.b64 * RANDOM_STATE_MUL;
}

// Box–Muller transform
double RandomGenerator::RandomNormal(double stdDev)
{
    // use cached number if possible
    if (this->lastRandomNormal != std::numeric_limits<double>::infinity())
    {
        double random          = this->lastRandomNormal;
        this->lastRandomNormal = std::numeric_limits<double>::infinity();

        return random * stdDev;
    }

    double r   = sqrt(-2.0 * log(1.0 - this->Random()));
    double phi = 2.0 * LOVE_M_PI * (1.0 - this->Random());

    this->lastRandomNormal = r * cos(phi);
    return r * sin(phi) * stdDev;
}

void RandomGenerator::SetSeed(RandomGenerator::Seed newSeed)
{
    this->seed = newSeed;

    // Xorshift isn't designed to give a good distribution of values across many
    // similar seeds, so we hash the state integer before using it.
    // http://www.reedbeta.com/blog/2013/01/12/quick-and-easy-gpu-random-numbers-in-d3d11/
    // Xorshift also can't handle a state value of 0, so we avoid that.
    do
    {
        this->seed.b64 = wangHash64(newSeed.b64);
    } while (newSeed.b64 == 0);

    this->randomState = newSeed;

    this->lastRandomNormal = std::numeric_limits<double>::infinity();
}

RandomGenerator::Seed RandomGenerator::GetSeed() const
{
    return this->seed;
}

void RandomGenerator::SetState(const std::string& stateString)
{
    // For this implementation we'll accept a hex string representing the
    // 64-bit state integer xorshift uses.

    // Hex string must start with 0x.
    if (stateString.find("0x") != 0 || stateString.size() < 3)
        throw love::Exception("Invalid random state: %s", stateString.c_str());

    Seed state = {};

    char* end = nullptr;
    state.b64 = strtoull(stateString.c_str(), &end, 16);

    if (end != nullptr && *end != 0)
        throw love::Exception("Invalid random state: %s", stateString.c_str());

    this->randomState      = state;
    this->lastRandomNormal = std::numeric_limits<double>::infinity();
}

std::string_view RandomGenerator::GetState() const
{
    // For this implementation we'll return a hex string representing the 64-bit
    // state integer xorshift uses.
    char result[255];
    snprintf(result, sizeof(result), "0x%x", this->randomState.b64);

    return result;
}
