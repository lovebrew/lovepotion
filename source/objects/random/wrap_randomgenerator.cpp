#include "objects/random/wrap_randomgenerator.h"

#include "wrap_randomgenerator_lua.h"

using namespace love;

template<typename T>
static T CheckRandomSeed_Part(lua_State* L, int index)
{
    double num = luaL_checknumber(L, index);
    double inf = std::numeric_limits<double>::infinity();

    // Disallow conversions from infinity and NaN.
    if (num == inf || num == -inf || num != num)
        luaL_argerror(L, index, "invalid random seed");

    return (T)num;
}

int Wrap_RandomGenerator::GetSeed(lua_State* L)
{
    RandomGenerator* self      = Wrap_RandomGenerator::CheckRandomGenerator(L, 1);
    RandomGenerator::Seed seed = self->GetSeed();

    lua_pushnumber(L, (lua_Number)seed.b32.low);
    lua_pushnumber(L, (lua_Number)seed.b32.high);

    return 2;
}

int Wrap_RandomGenerator::GetState(lua_State* L)
{
    RandomGenerator* self = Wrap_RandomGenerator::CheckRandomGenerator(L, 1);
    std::string state     = self->GetState();

    lua_pushlstring(L, state.data(), state.size());

    return 1;
}

int Wrap_RandomGenerator::_Random(lua_State* L)
{
    RandomGenerator* self = Wrap_RandomGenerator::CheckRandomGenerator(L, 1);

    lua_pushnumber(L, self->Random());

    return 1;
}

int Wrap_RandomGenerator::RandomNormal(lua_State* L)
{
    RandomGenerator* self = Wrap_RandomGenerator::CheckRandomGenerator(L, 1);

    double stddev = luaL_optnumber(L, 2, 1.0);
    double mean   = luaL_optnumber(L, 3, 0.0);

    double random = self->RandomNormal(stddev);

    lua_pushnumber(L, random + mean);

    return 1;
}

int Wrap_RandomGenerator::SetSeed(lua_State* L)
{
    RandomGenerator* self = Wrap_RandomGenerator::CheckRandomGenerator(L, 1);

    Luax::CatchException(L, [&]() { self->SetSeed(Wrap_RandomGenerator::CheckRandomSeed(L, 2)); });

    return 0;
}

int Wrap_RandomGenerator::SetState(lua_State* L)
{
    RandomGenerator* self = Wrap_RandomGenerator::CheckRandomGenerator(L, 1);

    Luax::CatchException(L, [&]() { self->SetState(luaL_checkstring(L, 2)); });

    return 0;
}

RandomGenerator::Seed Wrap_RandomGenerator::CheckRandomSeed(lua_State* L, int index)
{
    RandomGenerator::Seed seed;

    if (!lua_isnoneornil(L, index + 1))
    {
        seed.b32.low  = CheckRandomSeed_Part<uint32_t>(L, index);
        seed.b32.high = CheckRandomSeed_Part<uint32_t>(L, index + 1);
    }
    else
        seed.b64 = CheckRandomSeed_Part<uint64_t>(L, index);

    return seed;
}

RandomGenerator* Wrap_RandomGenerator::CheckRandomGenerator(lua_State* L, int index)
{
    return Luax::CheckType<RandomGenerator>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "_random",      Wrap_RandomGenerator::_Random      },
    { "getSeed",      Wrap_RandomGenerator::GetSeed      },
    { "getState",     Wrap_RandomGenerator::GetState     },
    { "randomNormal", Wrap_RandomGenerator::RandomNormal },
    { "setSeed",      Wrap_RandomGenerator::SetSeed      },
    { "setState",     Wrap_RandomGenerator::SetState     },
    { 0,              0                                  }
};
// clang-format on

int Wrap_RandomGenerator::Register(lua_State* L)
{
    int ret = Luax::RegisterType(L, &RandomGenerator::type, functions, nullptr);

    Luax::RunWrapper(L, (const char*)wrap_randomgenerator_lua, wrap_randomgenerator_lua_size,
                     "wrap_randomgenerator.lua", RandomGenerator::type);

    return ret;
}
