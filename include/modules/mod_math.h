#pragma once

#include "common/vector2.h"
#include <list>

namespace Math
{
    //Löve2D Functions

    int GammaToLinear(lua_State * L);

    int GetRandomSeed(lua_State * L);

    int GetRandomState(lua_State * L);

    int IsConvex(lua_State * L);

    int LinearToGamma(lua_State * L);

    int Noise(lua_State * L);

    int Random(lua_State * L);

    int RandomNormal(lua_State * L);

    int SetRandomSeed(lua_State * L);

    int SetRandomState(lua_State * L);

    int Triangulate(lua_State * L);

    //End Löve2D Functions

    //Helper functions
    
    //Gamma
    float convertGammaToLinear(float c);
    float convertLinearToGamma(float c);
    int getGammaArgs(lua_State * L, float color[4]);
    //End Gamma

    //Triangle
    bool onSameSide(const Vector2 &a, const Vector2 &b, const Vector2 &c, const Vector2 &d);
    bool pointInTriangle(const Vector2 &p, const Vector2 &a, const Vector2 &b, const Vector2 &c);
    bool isOrientedCCW(const Vector2 &a, const Vector2 &b, const Vector2 &c);
    bool isEar(const Vector2 &a, const Vector2 &b, const Vector2 &c, const std::list<const Vector2 *> &vertices);
    bool anyPointInTriangle(const std::list<const Vector2 *> &vertices, const Vector2 &a, const Vector2 &b, const Vector2 &c);
    //End Triangle

    //End Helper functions

    int Register(lua_State * L);
}