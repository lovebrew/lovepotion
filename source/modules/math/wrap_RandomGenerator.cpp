#include "modules/math/wrap_RandomGenerator.hpp"

#include <algorithm>
#include <cmath>

static constexpr char rng_lua[] = {
#include "modules/math/wrap_RandomGenerator.lua"
};

using namespace love;

int Wrap_RandomGenerator::_random(lua_State* L)
{
    auto* self = luax_checkrandomgenerator(L, 1);

    lua_pushnumber(L, self->random());

    return 1;
}

int Wrap_RandomGenerator::randomNormal(lua_State* L)
{
    auto* self    = luax_checkrandomgenerator(L, 1);
    double stddev = luaL_optnumber(L, 2, 1.0);
    double mean   = luaL_optnumber(L, 3, 0.0);

    double r = self->randomNormal(stddev);

    lua_pushnumber(L, r + mean);

    return 1;
}

int Wrap_RandomGenerator::setSeed(lua_State* L)
{
    auto* self = luax_checkrandomgenerator(L, 1);

    luax_catchexcept(L, [&] { self->setSeed(luax_checkrandomseed(L, 2)); });

    return 0;
}

int Wrap_RandomGenerator::getSeed(lua_State* L)
{
    auto* self = luax_checkrandomgenerator(L, 1);

    RandomGenerator::Seed seed = self->getSeed();

    lua_pushnumber(L, seed.b32.low);
    lua_pushnumber(L, seed.b32.high);

    return 2;
}

int Wrap_RandomGenerator::setState(lua_State* L)
{
    auto* self = luax_checkrandomgenerator(L, 1);

    luax_catchexcept(L, [&] { self->setState(luaL_checkstring(L, 2)); });

    return 0;
}

int Wrap_RandomGenerator::getState(lua_State* L)
{
    auto* self = luax_checkrandomgenerator(L, 1);

    std::string state = self->getState();

    luax_pushstring(L, state);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "_random",      Wrap_RandomGenerator::_random      },
    { "randomNormal", Wrap_RandomGenerator::randomNormal },
    { "setSeed",      Wrap_RandomGenerator::setSeed      },
    { "getSeed",      Wrap_RandomGenerator::getSeed      },
    { "setState",     Wrap_RandomGenerator::setState     },
    { "getState",     Wrap_RandomGenerator::getState     }
};
// clang-format on

namespace love
{
    RandomGenerator* luax_checkrandomgenerator(lua_State* L, int index)
    {
        return luax_checktype<RandomGenerator>(L, index);
    }

    template<typename T>
    static T checkrandomseed_part(lua_State* L, int index)
    {
        double number   = luaL_checknumber(L, index);
        double infinity = std::numeric_limits<double>::infinity();

        if (number == infinity || number == -infinity || number != number)
            luaL_argerror(L, index, "invalid random seed.");

        return (T)number;
    }

    RandomGenerator::Seed luax_checkrandomseed(lua_State* L, int index)
    {
        RandomGenerator::Seed seed {};

        if (!lua_isnoneornil(L, index + 1))
        {
            seed.b32.low  = checkrandomseed_part<uint32_t>(L, index);
            seed.b32.high = checkrandomseed_part<uint32_t>(L, index + 1);
        }
        else
            seed.b64 = checkrandomseed_part<uint64_t>(L, index);

        return seed;
    }

    int open_randomgenerator(lua_State* L)
    {
        int result = luax_register_type(L, &RandomGenerator::type, functions);

        luax_runwrapper(L, rng_lua, sizeof(rng_lua), "RandomGenerator.lua", RandomGenerator::type);

        return result;
    }
} // namespace love
