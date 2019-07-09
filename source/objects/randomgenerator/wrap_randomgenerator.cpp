#include "common/runtime.h"

#include "objects/randomgenerator/randomgenerator.h"
#include "objects/randomgenerator/wrap_randomgenerator.h"
#include "modules/mod_math.h"

#define CLASS_TYPE LUAOBJ_TYPE_RANDOMGENERATOR
#define CLASS_NAME "RandomGenerator"

int randomgeneratorNew(lua_State * L)
{
    RandomGenerator::Seed s;
    int top = lua_gettop(L);
    bool setseed = false;
    if (top > 0)
    {
        // User might've included more arguments than 2, but Lua's default
        // behavior is to ignore extra arguments, so we check for *at least* 2.
        if (top >= 2)
        {
            s.b32.low = checkrandomseed_part<u32>(L, 1);
            s.b32.high = checkrandomseed_part<u32>(L, 2);
        }
        else // Had to have been only 1 argument.
            s.b64 = checkrandomseed_part<u64>(L, 1);
        
        setseed = true;
    }

    void * raw_self = luaobj_newudata(L, sizeof(RandomGenerator));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    if (setseed)
        new (raw_self) RandomGenerator(s);
    else
        new (raw_self) RandomGenerator();
    
    return 1;
}

int randomgeneratorGetSeed(lua_State * L)
{
    RandomGenerator * self = (RandomGenerator *)luaobj_checkudata(L, 1, CLASS_TYPE);

    RandomGenerator::Seed s = self->getSeed();

    lua_pushnumber(L, (lua_Number) s.b32.low);
    lua_pushnumber(L, (lua_Number) s.b32.high);

    return 2;
}

int randomgeneratorGetState(lua_State * L)
{
    RandomGenerator * self = (RandomGenerator *)luaobj_checkudata(L, 1, CLASS_TYPE);

    std::string s = self->getState();

    lua_pushlstring(L, s.c_str(), s.length());
    
    return 1;
}

int randomgeneratorRandom(lua_State * L)
{
    RandomGenerator * self = (RandomGenerator *)luaobj_checkudata(L, 1, CLASS_TYPE);

    double n = 0;
    int top = lua_gettop(L);

    // The index would be zero, but the object takes up the first spot in the
    // stack, so all of these are incremented by 1.
    if (top > 1)
    {
        if (top >= 3)
        {
            double low = luaL_checknumber(L, 2);
            double high = luaL_checknumber(L, 3);
            n = self->random(low, high);
        }
        else
        {
            double high = luaL_checknumber(L, 2);
            n = self->random(high);
        }
    }
    else // No args, just a regular 0 to 1 RNG
    {
        n = self->random();
    }
    
    lua_pushnumber(L, n);

    return 1;
}

int randomgeneratorRandomNormal(lua_State * L)
{
    RandomGenerator * self = (RandomGenerator *)luaobj_checkudata(L, 1, CLASS_TYPE);

    double stddev = luaL_optnumber(L, 2, 1.0);
    double mean = luaL_optnumber(L, 3, 0.0);

    double r = self->randomNormal(stddev);

    lua_pushnumber(L, r + mean);

    return 1;
}

int randomgeneratorSetSeed(lua_State * L)
{
    RandomGenerator * self = (RandomGenerator *)luaobj_checkudata(L, 1, CLASS_TYPE);

    RandomGenerator::Seed s;
    if (lua_gettop(L) >= 3)
    {
        s.b32.low = checkrandomseed_part<u32>(L, 2);
        s.b32.high = checkrandomseed_part<u32>(L, 3);
    }
    else
        s.b64 = checkrandomseed_part<u64>(L, 2);

    self->setSeed(s);

    return 0;
}

int randomgeneratorSetState(lua_State * L)
{
    RandomGenerator * self = (RandomGenerator *)luaobj_checkudata(L, 1, CLASS_TYPE);

    self->setState(luaL_checkstring(L, 2));

    return 0;
}
int randomgeneratorGC(lua_State * L)
{
    RandomGenerator * self = (RandomGenerator *)luaobj_checkudata(L, 1, CLASS_TYPE);

    self->~RandomGenerator();
    
    return 0;
}

int randomgeneratorToString(lua_State * L)
{
    RandomGenerator * self = (RandomGenerator *)luaobj_checkudata(L, 1, CLASS_TYPE);

    char * data = self->ToString();

    lua_pushstring(L, data);

    free(data);

    return 1;
}


int initRandomGeneratorClass(lua_State *L) {

    luaL_Reg reg[] = 
    {
        { "__gc",         randomgeneratorGC           },
        { "__tostring",   randomgeneratorToString     },
        { "getSeed",      randomgeneratorGetSeed      },
        { "getState",     randomgeneratorGetState     },
        { "random",       randomgeneratorRandom       },
        { "randomNormal", randomgeneratorRandomNormal },
        { "setSeed",      randomgeneratorSetSeed      },
        { "setState",     randomgeneratorSetState     },
        { 0, 0 }
    };

    luaobj_newclass(L, CLASS_NAME, NULL, randomgeneratorNew, reg);

    return 1;

}
