#pragma once

#include <common/luax.hpp>
#include <objects/joint/joint.hpp>

namespace Wrap_Joint
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

    extern const luaL_Reg jointFunctions[0x0A];

    int Register(lua_State* L);
} // namespace Wrap_Joint
