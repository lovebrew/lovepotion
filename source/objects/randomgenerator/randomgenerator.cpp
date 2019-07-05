// This implementation was taken from https://bitbucket.org/rude/love/
// and was modified to work with Löve Potion. 

#include "common/runtime.h"

#include "objects/randomgenerator/randomgenerator.h"

#include <limits>

// Thomas Wang's 64-bit integer hashing function:
// https://web.archive.org/web/20110807030012/http://www.cris.com/%7ETtwang/tech/inthash.htm
static u64 wangHash64(u64 key)
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

RandomGenerator::RandomGenerator() : Object("RandomGenerator")
{
    this->last_randomnormal = std::numeric_limits<double>::infinity();
    Seed newseed;
    newseed.b32.low = 0xCBBF7A44;
    newseed.b32.high = 0x0139408D;
    this->setSeed(newseed);
}

RandomGenerator::RandomGenerator(Seed s) : Object("RandomGenerator")
{
    this->last_randomnormal = std::numeric_limits<double>::infinity();
    this->setSeed(s);
}

// 64 bit Xorshift implementation taken from the end of Sec. 3 (page 4) in
// George Marsaglia, "Xorshift RNGs", Journal of Statistical Software, Vol.8 (Issue 14), 2003
// Use an 'Xorshift*' variant, as shown here: http://xorshift.di.unimi.it

u64 RandomGenerator::rand()
{
    this->rng_state.b64 ^= (this->rng_state.b64 >> 12);
    this->rng_state.b64 ^= (this->rng_state.b64 << 25);
    this->rng_state.b64 ^= (this->rng_state.b64 >> 27);
    return this->rng_state.b64 * 2685821657736338717ULL;
}

double RandomGenerator::random()
{
    u64 r = rand();

    // From http://xoroshiro.di.unimi.it
    union { u64 i; double d; } u;
    u.i = ((0x3FFULL) << 52) | (r >> 12);

    return u.d - 1.0;
}

double RandomGenerator::random(double max)
{
    return floor(random() * max) + 1;
}

double RandomGenerator::random(double min, double max)
{
    return floor(random() * (max - min + 1)) + min;
}

// Box–Muller transform
double RandomGenerator::randomNormal(double stddev)
{
    // use cached number if possible
    if (last_randomnormal != std::numeric_limits<double>::infinity())
    {
        double r = last_randomnormal;
        last_randomnormal = std::numeric_limits<double>::infinity();
        return r * stddev;
    }

    double r   = sqrt(-2.0 * log(1. - random()));
    double phi = 2.0 * M_PI * (1. - random());

    last_randomnormal = r * cos(phi);
    return r * sin(phi) * stddev;
}

void RandomGenerator::setSeed(RandomGenerator::Seed newseed)
{
    this->seed = newseed;

    // Xorshift isn't designed to give a good distribution of values across many
    // similar seeds, so we hash the state integer before using it.
    // http://www.reedbeta.com/blog/2013/01/12/quick-and-easy-gpu-random-numbers-in-d3d11/
    // Xorshift also can't handle a state value of 0, so we avoid that.
    do
    {
        newseed.b64 = wangHash64(newseed.b64);
    } while (newseed.b64 == 0);

    this->rng_state = newseed;
}

RandomGenerator::Seed RandomGenerator::getSeed() const
{
    return seed;
}

void RandomGenerator::setState(const std::string &statestr)
{
    // For this implementation we'll accept a hex string representing the
    // 64-bit state integer xorshift uses.

    // Hex string must start with 0x.
    if (statestr.find("0x") != 0 || statestr.size() < 3)
        Love::RaiseError("Invalid random state: %s", statestr.c_str());

    Seed state = {};

    char *end = nullptr;
    state.b64 = strtoull(statestr.c_str(), &end, 16);

    if (end != nullptr && *end != 0)
        Love::RaiseError("Invalid random state: %s", statestr.c_str());

    this->rng_state = state;
}

std::string RandomGenerator::getState() const
{
    // For this implementation we'll return a hex string representing the 64-bit
    // state integer xorshift uses.

    char state[19]; // "0x" + 16 zero padded hex of rng_state.b64 + "\0"

    // For some reason on Switch, u64 is an unsigned long int, but on 3DS its an unsigned long long int
#if defined(__SWITCH__)
    sprintf(state, "0x%016lx", rng_state.b64);
#elif defined(_3DS)
    sprintf(state, "0x%0.16llx", rng_state.b64);
#endif

    return std::string(state);
}
