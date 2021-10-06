#pragma once

#include "common/luax.h"
#include "objects/texture/texture.h"
namespace Wrap_Texture
{
    int GetTextureType(lua_State* L);

    int GetWidth(lua_State* L);

    int GetHeight(lua_State* L);

    int GetDimensions(lua_State* L);

    int SetFilter(lua_State* L);

    int GetFilter(lua_State* L);

    int SetWrap(lua_State* L);

    int GetWrap(lua_State* L);

    extern const luaL_Reg functions[9];

    love::Texture* CheckTexture(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_Texture
