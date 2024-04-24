#pragma once

#include "common/luax.hpp"
#include "modules/math/BezierCurve.hpp"

namespace love
{
    BezierCurve* luax_checkbeziercurve(lua_State* L, int index);

    int open_beziercurve(lua_State* L);
} // namespace love

namespace Wrap_BezierCurve
{
    int getDegree(lua_State* L);

    int getDerivative(lua_State* L);

    int getControlPoint(lua_State* L);

    int setControlPoint(lua_State* L);

    int insertControlPoint(lua_State* L);

    int removeControlPoint(lua_State* L);

    int getControlPointCount(lua_State* L);

    int translate(lua_State* L);

    int rotate(lua_State* L);

    int scale(lua_State* L);

    int evaluate(lua_State* L);

    int getSegment(lua_State* L);

    int render(lua_State* L);

    int renderSegment(lua_State* L);
} // namespace Wrap_BezierCurve
