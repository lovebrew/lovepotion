#pragma once

#include "objects/random/randomgenerator.h"
#include "objects/random/wrap_randomgenerator.h"

#include "modules/math/mathmodule.h"

namespace Wrap_Math
{
    int GetRandomGenerator(lua_State * L);

    // int GammaToLinear(lua_State * L);

    int IsConvex(lua_State * L);

    // int LinearToGamma(lua_State * L);

    int NewRandomGenerator(lua_State * L);

    int Noise(lua_State * L);

    int Triangulate(lua_State * L);

    int Register(lua_State * L);
}
