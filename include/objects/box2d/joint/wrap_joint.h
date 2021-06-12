#pragma once

#include "common/luax.h"
#include "joint/joint.h"

namespace Wrap_Joint // ( ͡° ͜ʖ ͡°)
{
    void PushJoint(lua_State* L, love::Joint* joint);

    love::Joint* CheckJoint(lua_State* L, int index);

    int GetType(lua_State* L);

    int GetBodies(lua_State* L);

    int GetAnchors(lua_State* L);

    int GetReactionForce(lua_State* L);

    int GetReactionTorque(lua_State* L);

    int GetCollideConnected(lua_State* L);

    int SetUserdata(lua_State* L);

    int GetUserdata(lua_State* L);

    int Destroy(lua_State* L);

    int IsDestroyed(lua_State* L);

    int Register(lua_State* L);

    extern const luaL_Reg functions[11];
} // namespace Wrap_Joint
