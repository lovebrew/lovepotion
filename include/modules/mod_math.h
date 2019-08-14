/*
** mod_math.h
** @brief   : Math module for miscellaneous calculations
*/

#pragma once

#include "objects/randomgenerator/randomgenerator.h"
#include "common/vector2.h"
#include <list>

class Math
{
    public:
        Math() = delete;

        static int Register(lua_State * L);

    private:
        // Our own personal RNG!
        static inline RandomGenerator rng;

        // Löve2D Functions

        static int GammaToLinear(lua_State * L);

        static int GetRandomSeed(lua_State * L);

        static int GetRandomState(lua_State * L);

        static int IsConvex(lua_State * L);

        static int LinearToGamma(lua_State * L);

        static int Noise(lua_State * L);

        static int Random(lua_State * L);

        static int RandomNormal(lua_State * L);

        static int SetRandomSeed(lua_State * L);

        static int SetRandomState(lua_State * L);

        static int Triangulate(lua_State * L);

        //End Löve2D Functions

        //Helper functions

        //Gamma

        static float convertGammaToLinear(float c);

        static float convertLinearToGamma(float c);

        static int getGammaArgs(lua_State * L, float color[4]);

        //End Gamma

        //Triangle

        static bool onSameSide(const Vector2 & a, const Vector2 & b, const Vector2 & c, const Vector2 & d);

        static bool pointInTriangle(const Vector2 & p, const Vector2 & a, const Vector2 & b, const Vector2 & c);

        static bool isOrientedCCW(const Vector2 & a, const Vector2 & b, const Vector2 & c);

        static bool isEar(const Vector2 & a, const Vector2 & b, const Vector2 & c, const std::list<const Vector2 *> & vertices);

        static bool anyPointInTriangle(const std::list<const Vector2 *> & vertices, const Vector2 & a, const Vector2 & b, const Vector2 & c);

        //End Triangle

        //End Helper functions
};
