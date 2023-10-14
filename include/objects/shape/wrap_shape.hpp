#pragma once

#include <common/luax.hpp>
#include <objects/shape/shape.hpp>

#include <modules/physics/wrap_physics.hpp>

namespace Wrap_Shape
{
    void PushShape(lua_State* L, love::Shape* shape);

    love::Shape* CheckShape(lua_State* L, int index);

    int GetType(lua_State* L);

    int GetRadius(lua_State* L);

    int GetChildCount(lua_State* L);

    int SetFriction(lua_State* L);

    int SetRestitution(lua_State* L);

    int SetDensity(lua_State* L);

    int SetSensor(lua_State* L);

    int GetFriction(lua_State* L);

    int GetRestitution(lua_State* L);

    int GetDensity(lua_State* L);

    int IsSensor(lua_State* L);

    int GetBody(lua_State* L);

    int TestPoint(lua_State* L);

    int RayCast(lua_State* L);

    int ComputeAABB(lua_State* L);

    int ComputeMass(lua_State* L);

    int SetFilterData(lua_State* L);

    int GetFilterData(lua_State* L);

    int SetCategory(lua_State* L);

    int GetCategory(lua_State* L);

    int SetMask(lua_State* L);

    int GetMask(lua_State* L);

    int SetUserdata(lua_State* L);

    int GetUserdata(lua_State* L);

    int GetBoundingBox(lua_State* L);

    int GetMassData(lua_State* L);

    int GetGroupIndex(lua_State* L);

    int SetGroupIndex(lua_State* L);

    int Destroy(lua_State* L);

    int IsDestroyed(lua_State* L);

    extern const luaL_Reg shapeFunctions[0x1E];

    int Register(lua_State* L);
} // namespace Wrap_Shape
