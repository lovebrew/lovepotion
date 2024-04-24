#pragma once

#include "common/luax.hpp"
#include "modules/math/MathModule.hpp"

namespace Wrap_MathModule
{
    int _getRandomGenerator(lua_State* L);

    int newRandomGenerator(lua_State* L);

    int newBezierCurve(lua_State* L);

    int newTransform(lua_State* L);

    int triangulate(lua_State* L);

    int isConvex(lua_State* L);

    int gammaToLinear(lua_State* L);

    int linearToGamma(lua_State* L);

    int perlinNoise(lua_State* L);

    int simplexNoise(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_MathModule
