#pragma once

#include <common/luax.hpp>
#include <objects/spritebatch/spritebatch.hpp>

namespace Wrap_SpriteBatch
{
    love::SpriteBatch* CheckSpriteBatch(lua_State* L, int index);

    int Add(lua_State* L);

    int Set(lua_State* L);

    int Clear(lua_State* L);

    int Flush(lua_State* L);

    int SetTexture(lua_State* L);

    int GetTexture(lua_State* L);

    int SetColor(lua_State* L);

    int GetColor(lua_State* L);

    int GetCount(lua_State* L);

    int GetBufferSize(lua_State* L);

    int SetDrawRange(lua_State* L);

    int GetDrawRange(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_SpriteBatch