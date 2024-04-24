#pragma once

#include "common/luax.hpp"
#include "modules/math/RandomGenerator.hpp"

namespace love
{
    RandomGenerator* luax_checkrandomgenerator(lua_State* L, int index);

    RandomGenerator::Seed luax_checkrandomseed(lua_State* L, int index);

    int open_randomgenerator(lua_State* L);
} // namespace love

namespace Wrap_RandomGenerator
{
    int _random(lua_State* L);

    int randomNormal(lua_State* L);

    int setSeed(lua_State* L);

    int getSeed(lua_State* L);

    int setState(lua_State* L);

    int getState(lua_State* L);
} // namespace Wrap_RandomGenerator
