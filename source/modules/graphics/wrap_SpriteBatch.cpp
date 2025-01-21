#include "modules/graphics/wrap_SpriteBatch.hpp"
#include "modules/graphics/wrap_Texture.hpp"

using namespace love;

namespace love
{
    static inline int w_SpriteBatch_add_or_set(lua_State* L, SpriteBatch* batch, int start, int index)
    {
        auto* quad = luax_totype<Quad>(L, start);

        if (quad != nullptr)
            start++;
        else if (lua_isnil(L, start) && !lua_isnoneornil(L, start + 1))
            return luax_typeerror(L, start, "Quad");

        luax_checkstandardtransform(L, start, [&](const Matrix4& matrix) {
            luax_catchexcept(L, [&]() {
                if (quad)
                    index = batch->add(quad, matrix, index);
                else
                    index = batch->add(matrix, index);
            });
        });

        return index;
    }
} // namespace love

int Wrap_SpriteBatch::add(lua_State* L)
{
    auto* self = luax_checkspritebatch(L, 1);

    int index = w_SpriteBatch_add_or_set(L, self, 2, -1);
    lua_pushinteger(L, index + 1);

    return 1;
}

int Wrap_SpriteBatch::addLayer(lua_State* L)
{
    return 0;
}

int Wrap_SpriteBatch::set(lua_State* L)
{
    auto* self = luax_checkspritebatch(L, 1);
    int index  = luaL_checkinteger(L, 2) - 1;

    w_SpriteBatch_add_or_set(L, self, 3, index);

    return 0;
}

int Wrap_SpriteBatch::setLayer(lua_State* L)
{
    return 0;
}

int Wrap_SpriteBatch::clear(lua_State* L)
{
    auto* self = luax_checkspritebatch(L, 1);
    self->clear();

    return 0;
}

int Wrap_SpriteBatch::flush(lua_State* L)
{
    auto* self = luax_checkspritebatch(L, 1);
    self->flush();

    return 0;
}

int Wrap_SpriteBatch::setTexture(lua_State* L)
{
    auto* self    = luax_checkspritebatch(L, 1);
    auto* texture = luax_checktexture(L, 2);

    luax_catchexcept(L, [&]() { self->setTexture(texture); });

    return 0;
}

int Wrap_SpriteBatch::getTexture(lua_State* L)
{
    auto* self = luax_checkspritebatch(L, 1);
    luax_pushtype(L, self->getTexture());

    return 1;
}

int Wrap_SpriteBatch::setColor(lua_State* L)
{
    auto* self = luax_checkspritebatch(L, 1);
    Color color {};

    if (lua_istable(L, 2))
    {
        for (int index = 1; index <= 4; index++)
            lua_rawgeti(L, 2, index);

        color.r = luax_checknumberclamped01(L, -4);
        color.g = luax_checknumberclamped01(L, -3);
        color.b = luax_checknumberclamped01(L, -2);
        color.a = luax_optnumberclamped01(L, -1, 1.0f);

        lua_pop(L, 4);
    }
    else
    {
        color.r = luax_checknumberclamped01(L, 2);
        color.g = luax_checknumberclamped01(L, 3);
        color.b = luax_checknumberclamped01(L, 4);
        color.a = luax_optnumberclamped01(L, 5, 1.0f);
    }

    self->setColor(color);

    return 0;
}

int Wrap_SpriteBatch::getColor(lua_State* L)
{
    auto* self  = luax_checkspritebatch(L, 1);
    Color color = self->getColor();

    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);

    return 4;
}

int Wrap_SpriteBatch::getCount(lua_State* L)
{
    auto* self = luax_checkspritebatch(L, 1);
    lua_pushinteger(L, self->getCount());

    return 1;
}

int Wrap_SpriteBatch::getBufferSize(lua_State* L)
{
    auto* self = luax_checkspritebatch(L, 1);
    lua_pushinteger(L, self->getBufferSize());

    return 1;
}

int Wrap_SpriteBatch::setDrawRange(lua_State* L)
{
    auto* self = luax_checkspritebatch(L, 1);

    if (lua_isnoneornil(L, 2))
        self->setDrawRange();
    else
    {
        int start = luaL_checkinteger(L, 2) - 1;
        int count = luaL_checkinteger(L, 3);

        luax_catchexcept(L, [&]() { self->setDrawRange(start, count); });
    }

    return 0;
}

int Wrap_SpriteBatch::getDrawRange(lua_State* L)
{
    auto* self = luax_checkspritebatch(L, 1);

    int start = 0;
    int count = 1;

    if (!self->getDrawRange(start, count))
        return 0;

    lua_pushnumber(L, start + 1);
    lua_pushnumber(L, count);

    return 2;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "add",           Wrap_SpriteBatch::add           },
    { "set",           Wrap_SpriteBatch::set           },
    { "addLayer",      Wrap_SpriteBatch::addLayer      },
    { "setLayer",      Wrap_SpriteBatch::setLayer      },
    { "clear",         Wrap_SpriteBatch::clear         },
    { "flush",         Wrap_SpriteBatch::flush         },
    { "setTexture",    Wrap_SpriteBatch::setTexture    },
    { "getTexture",    Wrap_SpriteBatch::getTexture    },
    { "setColor",      Wrap_SpriteBatch::setColor      },
    { "getColor",      Wrap_SpriteBatch::getColor      },
    { "getCount",      Wrap_SpriteBatch::getCount      },
    { "getBufferSize", Wrap_SpriteBatch::getBufferSize },
    { "setDrawRange",  Wrap_SpriteBatch::setDrawRange  },
    { "getDrawRange",  Wrap_SpriteBatch::getDrawRange  }
};
// clang-format on

namespace love
{
    SpriteBatch* luax_checkspritebatch(lua_State* L, int index)
    {
        return luax_checktype<SpriteBatch>(L, index);
    }

    int open_spritebatch(lua_State* L)
    {
        return luax_register_type(L, &SpriteBatch::type, functions);
    }
} // namespace love
