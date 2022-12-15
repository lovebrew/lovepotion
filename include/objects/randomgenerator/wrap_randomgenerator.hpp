#pragma once

#include <common/luax.hpp>
#include <objects/randomgenerator/randomgenerator.hpp>

namespace Wrap_RandomGenerator
{
    love::RandomGenerator* CheckRandomGenerator(lua_State* L);

    love::RandomGenerator::Seed CheckSeed(lua_State* L, int index);

    int __Random(lua_State* L);

    int RandomNormal(lua_State* L);

    int SetSeed(lua_State* L);

    int GetSeed(lua_State* L);

    int SetState(lua_State* L);

    int GetState(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_RandomGenerator
