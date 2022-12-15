#pragma once

#include <common/luax.hpp>

namespace Wrap_Math
{
    int GetRandomGenerator(lua_State* L);

    int NewRandomGenerator(lua_State* L);

    int NewBezierCurve(lua_State* L);

    int NewTransform(lua_State* L);

    int Triangulate(lua_State* L);

    int IsConvex(lua_State* L);

    int GammaToLinear(lua_State* L);

    int LinearToGamma(lua_State* L);

    int PerlinNoise(lua_State* L);

    int SimplexNoise(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Math
