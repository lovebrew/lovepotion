#pragma once

#include "common/luax.h"
#include "shape.h"

namespace Wrap_Shape
{
    int GetType(lua_State* L);

    int GetRadius(lua_State* L);

    int GetChildCount(lua_State* L);

    int TestPoint(lua_State* L);

    int RayCast(lua_State* L);

    int ComputeAABB(lua_State* L);

    int ComputeMass(lua_State* L);

    love::Shape* CheckShape(lua_State* L, int index);

    int Register(lua_State* L);

    extern const luaL_Reg functions[8];
} // namespace Wrap_Shape
