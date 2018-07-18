#include "common/runtime.h"
#include "modules/mod_math.h"

float RANDOM_SEED;

//love.math.setRandomSeed
int Math::SetRandomSeed(lua_State * L)
{
    RANDOM_SEED = luaL_checknumber(L, 1);

    lua_getfield(L, LUA_GLOBALSINDEX, "math");
    lua_getfield(L, -1, "randomseed");
    lua_remove(L, -2);
    
    lua_pushnumber(L, RANDOM_SEED);

    lua_call(L, 1, 0);

    return 0;
}


//love.math.random
int Math::Random(lua_State * L)
{
    int args = lua_gettop(L);

    float lower = luaL_optnumber(L, 1, 0);
    float upper = luaL_optnumber(L, 2, 1);

    lua_getfield(L, LUA_GLOBALSINDEX, "math");
    lua_getfield(L, -1, "random");
    lua_remove(L, -2);

    if (args == 0)
    {
        lua_call(L, 0, 1);
    }
    if (args == 1)
    {
        lua_pushnumber(L, upper);

        lua_call(L, 1, 1);
    }
    else if (args == 2)
    {
        lua_pushnumber(L, lower);
        lua_pushnumber(L, upper);
    
        lua_call(L, 2, 1);
    }


    return 1;
}


//love.math.getRandomSeed
int Math::GetRandomSeed(lua_State * L)
{
    lua_pushnumber(L, RANDOM_SEED);

    return 1;
}

int Math::Register(lua_State * L)
{
    luaL_Reg reg[] = 
    {
        { "getRandomSeed", GetRandomSeed },
        { "random",        Random        },
        { "setRandomSeed", SetRandomSeed },
        { 0, 0 }
    };

    luaL_newlib(L, reg);
    
    return 1;
}
