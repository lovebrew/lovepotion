#pragma once

#include <limits>

int randomgeneratorNew(lua_State * L);

int randomgeneratorToString(lua_State * L);

int randomgeneratorGetSeed(lua_State * L);

int randomgeneratorGetState(lua_State * L);

int randomgeneratorRandom(lua_State * L);

int randomgeneratorRandomNormal(lua_State * L);

int randomgeneratorSetSeed(lua_State * L);

int randomgeneratorSetState(lua_State * L);

int initRandomGeneratorClass(lua_State *L);

template <typename T>
static T checkrandomseed_part(lua_State *L, int idx)
{
    double num = luaL_checknumber(L, idx);
    double inf = std::numeric_limits<double>::infinity();

    // Disallow conversions from infinity and NaN.
    if (num == inf || num == -inf || num != num)
        luaL_argerror(L, idx, "invalid random seed");

    return (T) num;
}
