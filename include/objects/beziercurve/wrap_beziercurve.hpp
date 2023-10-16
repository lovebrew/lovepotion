#pragma once

#include <common/luax.hpp>
#include <objects/beziercurve/beziercurve.hpp>

namespace Wrap_BezierCurve
{
    love::BezierCurve* CheckBezierCurve(lua_State* L, int index);

    int GetDegree(lua_State* L);

    int GetDerivative(lua_State* L);

    int GetControlPoint(lua_State* L);

    int SetControlPoint(lua_State* L);

    int InsertControlPoint(lua_State* L);

    int RemoveControlPoint(lua_State* L);

    int GetControlPointCount(lua_State* L);

    int Translate(lua_State* L);

    int Rotate(lua_State* L);

    int Scale(lua_State* L);

    int Evaluate(lua_State* L);

    int GetSegment(lua_State* L);

    int Render(lua_State* L);

    int RenderSegment(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_BezierCurve
