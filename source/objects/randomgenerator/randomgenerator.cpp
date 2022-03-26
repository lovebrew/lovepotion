#include "objects/random/randomgenerator.h"

#include <iomanip>
#include <sstream>

using namespace love;

love::Type RandomGenerator::type("RandomGenerator", &Object::type);

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
    Seed newSeed;
    newSeed.b32.low  = 0xCBBF7A44;
    newSeed.b32.high = 0x0139408D;

    this->SetSeed(newSeed);
}

u64 RandomGenerator::UniformRandom()
{
    this->state.b64 ^= (this->state.b64 >> 12);
    this->state.b64 ^= (this->state.b64 << 25);
    this->state.b64 ^= (this->state.b64 >> 27);

    return this->state.b64 * 2685821657736338717ULL;
}

double RandomGenerator::RandomNormal(double stddev)
{
    if (this->lastRandomNormal != std::numeric_limits<double>::infinity())
    {
        double random          = this->lastRandomNormal;
        this->lastRandomNormal = std::numeric_limits<double>::infinity();

        return random * stddev;
    }

    double random = sqrt(-2.0 * log(1.0 - this->Random()));
    double phi    = 2.0 * M_PI * (1.0 - this->Random());

    this->lastRandomNormal = random * cos(phi);

    return random * sin(phi) * stddev;
}

void RandomGenerator::SetSeed(Seed newSeed)
{
    this->seed = newSeed;

    do
    {
        newSeed.b64 = wangHash64(newSeed.b64);
    } while (newSeed.b64 == 0);

    this->state            = newSeed;
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
    char* end  = nullptr;

    this->state.b64 = strtoull(stateString.c_str(), &end, 16);

    if (end != nullptr && *end != 0)
        throw love::Exception("Invalid random state: %s", stateString.c_str());

    this->state            = state;
    this->lastRandomNormal = std::numeric_limits<double>::infinity();
}

std::string RandomGenerator::GetState() const
{
    std::stringstream ss;
    ss << "0x" << std::setfill('0') << std::setw(16) << std::hex << this->state.b64;

    return ss.str();
}
