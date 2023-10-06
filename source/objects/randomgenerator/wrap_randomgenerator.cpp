#include <objects/randomgenerator/wrap_randomgenerator.hpp>

using namespace love;

static constexpr char wrap_randomgenerator_lua[] = {
#include "scripts/wrap_randomgenerator.lua"
};

template<typename T>
static T checkRandomSeed_Part(lua_State* L, int index)
{
    double number   = luaL_checknumber(L, index);
    double infinity = std::numeric_limits<double>::infinity();

    if (number == infinity || number == -infinity || number != number)
        luaL_argerror(L, index, "invalid random seed");

    return (T)number;
}

RandomGenerator::Seed Wrap_RandomGenerator::CheckSeed(lua_State* L, int index)
{
    RandomGenerator::Seed seed {};

    if (!lua_isnoneornil(L, index + 1))
    {
        seed.b32.low  = checkRandomSeed_Part<uint32_t>(L, index);
        seed.b32.high = checkRandomSeed_Part<uint32_t>(L, index + 1);
    }
    else
        seed.b64 = checkRandomSeed_Part<uint64_t>(L, index);

    return seed;
}

RandomGenerator* Wrap_RandomGenerator::CheckRandomGenerator(lua_State* L, int index)
{
    return luax::CheckType<RandomGenerator>(L, index);
}

int Wrap_RandomGenerator::__Random(lua_State* L)
{
    auto* self = Wrap_RandomGenerator::CheckRandomGenerator(L, 1);

    lua_pushnumber(L, self->Random());

    return 1;
}

int Wrap_RandomGenerator::RandomNormal(lua_State* L)
{
    auto* self = Wrap_RandomGenerator::CheckRandomGenerator(L, 1);

    double stdDev = luaL_optnumber(L, 2, 1.0);
    double mean   = luaL_optnumber(L, 3, 0.0);
    double random = self->RandomNormal(stdDev);

    lua_pushnumber(L, random + mean);

    return 1;
}

int Wrap_RandomGenerator::SetSeed(lua_State* L)
{
    auto* self = Wrap_RandomGenerator::CheckRandomGenerator(L, 1);

    luax::CatchException(L, [&]() { self->SetSeed(Wrap_RandomGenerator::CheckSeed(L, 2)); });

    return 0;
}

int Wrap_RandomGenerator::GetSeed(lua_State* L)
{
    auto* self      = Wrap_RandomGenerator::CheckRandomGenerator(L, 1);
    const auto seed = self->GetSeed();

    lua_pushnumber(L, (lua_Number)seed.b32.low);
    lua_pushnumber(L, (lua_Number)seed.b32.high);

    return 2;
}

int Wrap_RandomGenerator::SetState(lua_State* L)
{
    auto* self = Wrap_RandomGenerator::CheckRandomGenerator(L, 1);

    luax::CatchException(L, [&]() { self->SetState(luax::CheckString(L, 2)); });

    return 0;
}

int Wrap_RandomGenerator::GetState(lua_State* L)
{
    auto* self = Wrap_RandomGenerator::CheckRandomGenerator(L, 1);

    luax::PushString(L, self->GetState());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "_random",      Wrap_RandomGenerator::__Random     },
    { "getSeed",      Wrap_RandomGenerator::GetSeed      },
    { "getState",     Wrap_RandomGenerator::GetState     },
    { "randomNormal", Wrap_RandomGenerator::RandomNormal },
    { "setSeed",      Wrap_RandomGenerator::SetSeed      },
    { "setState",     Wrap_RandomGenerator::SetState     }
};
// clang-format on

int Wrap_RandomGenerator::Register(lua_State* L)
{
    int result = luax::RegisterType(L, &RandomGenerator::type, functions);

    luax::WrapObject(L, wrap_randomgenerator_lua, sizeof(wrap_randomgenerator_lua),
                     "wrap_randomgenerator.lua", RandomGenerator::type);

    return result;
}
