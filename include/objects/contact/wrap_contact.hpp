#pragma once

#include <common/luax.hpp>
#include <objects/contact/contact.hpp>

namespace Wrap_Contact
{
    love::Contact* CheckContact(lua_State* L, int index);

    int GetPositions(lua_State* L);

    int GetNormal(lua_State* L);

    int GetFriction(lua_State* L);

    int GetRestitution(lua_State* L);

    int IsEnabled(lua_State* L);

    int IsTouching(lua_State* L);

    int SetFriction(lua_State* L);

    int SetRestitution(lua_State* L);

    int SetEnabled(lua_State* L);

    int ResetFriction(lua_State* L);

    int ResetRestitution(lua_State* L);

    int SetTangentSpeed(lua_State* L);

    int GetTangentSpeed(lua_State* L);

    int GetChildren(lua_State* L);

    int GetShapes(lua_State* L);

    int GetFixtures(lua_State* L);

    int IsDestroyed(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Contact
