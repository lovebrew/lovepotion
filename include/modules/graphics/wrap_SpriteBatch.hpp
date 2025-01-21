#pragma once

#include "common/luax.hpp"
#include "modules/graphics/SpriteBatch.hpp"

#include "modules/math/Transform.hpp"

namespace love
{
    template<typename T>
    static void luax_checkstandardtransform(lua_State* L, int index, const T& func)
    {
        auto* transform = luax_totype<Transform>(L, index);

        if (transform != nullptr)
            func(transform->getMatrix());
        else
        {
            int nargs = lua_gettop(L);

            float x  = luaL_optnumber(L, index + 0, 0.0);
            float y  = luaL_optnumber(L, index + 1, 0.0);
            float a  = luaL_optnumber(L, index + 2, 0.0);
            float sx = luaL_optnumber(L, index + 3, 1.0);
            float sy = luaL_optnumber(L, index + 4, sx);

            float ox = 0.0f;
            float oy = 0.0f;

            if (nargs >= index + 5)
            {
                ox = luaL_optnumber(L, index + 5, 0.0);
                oy = luaL_optnumber(L, index + 6, 0.0);
            }

            float kx = 0.0f;
            float ky = 0.0f;

            if (nargs >= index + 7)
            {
                kx = luaL_optnumber(L, index + 7, 0.0);
                ky = luaL_optnumber(L, index + 8, 0.0);
            }

            func(love::Matrix4(x, y, a, sx, sy, ox, oy, kx, ky));
        }
    }

    SpriteBatch* luax_checkspritebatch(lua_State* L, int index);

    int open_spritebatch(lua_State* L);
} // namespace love

namespace Wrap_SpriteBatch
{
    int add(lua_State* L);

    int set(lua_State* L);

    int addLayer(lua_State* L);

    int setLayer(lua_State* L);

    int clear(lua_State* L);

    int flush(lua_State* L);

    int setTexture(lua_State* L);

    int getTexture(lua_State* L);

    int setColor(lua_State* L);

    int getColor(lua_State* L);

    int getCount(lua_State* L);

    int getBufferSize(lua_State* L);

    int setDrawRange(lua_State* L);

    int getDrawRange(lua_State* L);
} // namespace Wrap_SpriteBatch
