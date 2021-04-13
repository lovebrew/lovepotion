#pragma once

#include "common/luax.h"
#include "objects/random/randomgenerator.h"

namespace Wrap_RandomGenerator
{
    int GetSeed(lua_State* L);

    int GetState(lua_State* L);

    int _Random(lua_State* L);

    int RandomNormal(lua_State* L);

    int SetSeed(lua_State* L);

    int SetState(lua_State* L);

    love::RandomGenerator::Seed CheckRandomSeed(lua_State* L, int index);

    love::RandomGenerator* CheckRandomGenerator(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_RandomGenerator
