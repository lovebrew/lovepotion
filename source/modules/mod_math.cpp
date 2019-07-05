#include "common/runtime.h"
#include "modules/mod_math.h"

#include "objects/randomgenerator/randomgenerator.h"
#include "objects/randomgenerator/wrap_randomgenerator.h"

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

int Math::Register(lua_State * L)
{
    luaL_Reg reg[] = 
    {
    	{ "gammaToLinear",      GammaToLinear      },
        { "getRandomSeed",      GetRandomSeed      },
        { "linearToGamma",      LinearToGamma      },
        { "random",             Random             },
        { "setRandomSeed",      SetRandomSeed      },
		{ "newRandomGenerator", randomgeneratorNew },
        { 0, 0 }
    };

    luaL_newlib(L, reg);
    
    return 1;
}
