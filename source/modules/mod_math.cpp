#include "common/runtime.h"
#include "modules/mod_math.h"

#include "objects/randomgenerator/randomgenerator.h"
#include "objects/randomgenerator/wrap_randomgenerator.h"

#include <noise1234.h>
#include <simplexnoise1234.h>

#include <time.h>
#include <limits>

// Our own personal RNG!
RandomGenerator rng;

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

//love.math.setRandomSeed
int Math::SetRandomSeed(lua_State * L)
{
    RandomGenerator::Seed s;
    if (lua_gettop(L) >= 2)
    {
        s.b32.low = checkrandomseed_part<u32>(L, 1);
        s.b32.high = checkrandomseed_part<u32>(L, 2);
    }
    else
        s.b64 = checkrandomseed_part<u64>(L, 1);

    rng.setSeed(s);

    return 0;
}

//love.math.setRandomState
int Math::SetRandomState(lua_State * L)
{
    rng.setState(luaL_checkstring(L, 1));

    return 0;
}


//love.math.random
int Math::Random(lua_State * L)
{
    double n = 0;
    int top = lua_gettop(L);

    if (top > 0)
    {
        if (top >= 2)
        {
            double low = luaL_checknumber(L, 1);
            double high = luaL_checknumber(L, 2);
            n = rng.random(low, high);
        }
        else
        {
            double high = luaL_checknumber(L, 1);
            n = rng.random(high);
        }
    }
    else // No args, just a regular 0 to 1 RNG
    {
        n = rng.random();
    }
    
    lua_pushnumber(L, n);

    return 1;
}

//love.math.randomNormal
int Math::RandomNormal(lua_State * L)
{
    double stddev = luaL_optnumber(L, 1, 1.0);
    double mean = luaL_optnumber(L, 2, 0.0);

    double r = rng.randomNormal(stddev);

    lua_pushnumber(L, r + mean);

    return 1;
}

//love.math.getRandomSeed
int Math::GetRandomSeed(lua_State * L)
{
    RandomGenerator::Seed s = rng.getSeed();

    lua_pushnumber(L, s.b32.low);
    lua_pushnumber(L, s.b32.high);

    return 2;
}

//love.math.getRandomState
int Math::GetRandomState(lua_State * L)
{
    std::string s = rng.getState();

    lua_pushlstring(L, s.c_str(), s.length());

    return 1;
}


int convertGammaToLinear(float c)
{
    if (c <= 0.04045f)
        return c / 12.92f;
    else
        return powf((c + 0.055f) / 1.055f, 2.4f);
}

int convertLinearToGamma(float c)
{
    if (c <= 0.0031308f)
        return c * 12.92f;
    else
        return 1.055f * powf(c, 1.0f / 2.4f) - 0.055f;
}

int getGammaArgs(lua_State * L, float color[4])
{
    int numcomponents = 0;

    if (lua_istable(L, 1))
    {
        int n = (int) lua_objlen(L, 1);
        for (int i = 1; i <= n && i <= 4; i++)
        {
            lua_rawgeti(L, 1, i);
            color[i - 1] = (float) std::min(std::max(luaL_checknumber(L, -1), 0.0), 1.0);
            numcomponents++;
        }

        lua_pop(L, numcomponents);
    }
    else
    {
        int n = lua_gettop(L);
        for (int i = 1; i <= n && i <= 4; i++)
        {
            color[i - 1] = (float) std::min(std::max(luaL_checknumber(L, i), 0.0), 1.0);
            numcomponents++;
        }
    }

    if (numcomponents == 0)
        luaL_checknumber(L, 1);

    return numcomponents;
}

//love.math.gammaToLinear
int Math::GammaToLinear(lua_State * L)
{
    float color[4];
    int numcomponents = getGammaArgs(L, color);

    for (int i = 0; i < numcomponents; i++)
    {
        if (i < 3) // Alpha is always linear.
            color[i] = convertGammaToLinear(color[i]);
        lua_pushnumber(L, color[i]);
    }

    return numcomponents;
}

//love.math.linearToGamma
int Math::LinearToGamma(lua_State * L)
{
    float color[4];
    int numcomponents = getGammaArgs(L, color);

    for (int i = 0; i < numcomponents; i++)
    {
        if (i < 3) // Alpha is always linear.
            color[i] = convertLinearToGamma(color[i]);
        lua_pushnumber(L, color[i]);
    }

    return numcomponents;
}

//love.math.isConvex
int Math::IsConvex(lua_State * L)
{
    lua_pushboolean(L, false);

    return 1;
}

//love.math.noise
int Math::Noise(lua_State * L)
{
    int nargs = std::min(std::max(lua_gettop(L), 1), 4);
    float args[4];

    for (int i = 0; i < nargs; i++)
        args[i] = (float)luaL_checknumber(L, i + 1);

    float val = 0.0f;

    switch (nargs) // dimensions of noise vector
    {
        case 1:
            val = SimplexNoise1234::noise(args[0]) * 0.5f + 0.5f;
            break;
        case 2:
            val = SimplexNoise1234::noise(args[0], args[1]) * 0.5f + 0.5f;
            break;

        // Perlin noise is used instead of Simplex noise in the 3D and 4D cases to avoid patent issues.

        case 3:
            val = Noise1234::noise(args[0], args[1], args[2]) * 0.5f + 0.5f;
            break;
        case 4:
            val = Noise1234::noise(args[0], args[1], args[2], args[3]) * 0.5f + 0.5f;
            break;
    }

    lua_pushnumber(L, val);

    return 1;
}

//love.math.triangulate
int Math::Triangulate(lua_State * L)
{
    //luaL_pushtriangles(L);

    return 1;
}

int Math::Register(lua_State * L)
{
    // The module's random generator is always seeded with 
    RandomGenerator::Seed s;
    s.b64 = (u64) time(nullptr);
    rng.setSeed(s);

    luaL_Reg reg[] = 
    {
        { "gammaToLinear",      GammaToLinear      },
        { "getRandomSeed",      GetRandomSeed      },
        { "getRandomState",     GetRandomState     },
        { "isConvex",           IsConvex           },
        { "linearToGamma",      LinearToGamma      },
        // { "newBezierCurve",     beziercurveNew     },
        { "newRandomGenerator", randomgeneratorNew },
        // { "newTransform",       transformNew       },
        { "noise",              Noise              },
        { "random",             Random             },
        { "randomNormal",       RandomNormal       },
        { "setRandomSeed",      SetRandomSeed      },
        { "setRandomState",     SetRandomState     },
        { "triangulate",        Triangulate        },
        { 0, 0 }
    };

    luaL_newlib(L, reg);
    
    return 1;
}
