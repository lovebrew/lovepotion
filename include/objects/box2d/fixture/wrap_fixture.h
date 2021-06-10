#pragma once

#include "common/luax.h"

#include "fixture.h"

namespace Wrap_Fixture
{
    int GetType(lua_State* L);

    int SetFriction(lua_State* L);

    int SetRestitution(lua_State* L);

    int SetDensity(lua_State* L);

    int SetSensor(lua_State* L);

    int GetFriction(lua_State* L);

    int GetRestitution(lua_State* L);

    int GetDensity(lua_State* L);

    int IsSensor(lua_State* L);

    int GetBody(lua_State* L);

    int GetShape(lua_State* L);

    int TestPoint(lua_State* L);

    int RayCast(lua_State* L);

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

    love::Fixture* CheckFixture(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_Fixture
