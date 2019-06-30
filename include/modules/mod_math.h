#pragma once

namespace Math
{
    //Löve2D Functions

    int GammaToLinear(lua_State * L);

    int GetRandomSeed(lua_State * L);

    int LinearToGamma(lua_State * L);

    int Random(lua_State * L);

    int SetRandomSeed(lua_State * L);

    //End Löve2D Functions

    int Register(lua_State * L);
}