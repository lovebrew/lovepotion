#pragma once

#include "common/luax.h"
#include "world/world.h"

namespace Wrap_World
{
    int Update(lua_State* L);

    int SetCallbacks(lua_State* L);

    int GetCallbacks(lua_State* L);

    int SetContactFilter(lua_State* L);

    int GetContactFilter(lua_State* L);

    int SetGravity(lua_State* L);

    int GetGravity(lua_State* L);

    int TranslateOrigin(lua_State* L);

    int SetSleepingAllowed(lua_State* L);

    int IsSleepingAllowed(lua_State* L);

    int IsLocked(lua_State* L);

    int GetBodyCount(lua_State* L);

    int GetJointCount(lua_State* L);

    int GetContactCount(lua_State* L);

    int GetBodies(lua_State* L);

    int GetJoints(lua_State* L);

    int GetContacts(lua_State* L);

    int QueryBoundingBox(lua_State* L);

    int RayCast(lua_State* L);

    int Destroy(lua_State* L);

    int IsDestroyed(lua_State* L);

    love::World* CheckWorld(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_World
