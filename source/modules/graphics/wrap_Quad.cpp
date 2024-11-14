#include "modules/graphics/wrap_Quad.hpp"

using namespace love;

int Wrap_Quad::setViewport(lua_State* L)
{
    auto* self = luax_checkquad(L, 1);

    Quad::Viewport viewport {};
    viewport.x = luaL_checknumber(L, 2);
    viewport.y = luaL_checknumber(L, 3);
    viewport.w = luaL_checknumber(L, 4);
    viewport.h = luaL_checknumber(L, 5);

    if (lua_isnoneornil(L, 6))
        self->setViewport(viewport);
    else
    {
        double sw = luaL_checknumber(L, 6);
        double sh = luaL_checknumber(L, 7);

        self->refresh(viewport, sw, sh);
    }

    return 0;
}

int Wrap_Quad::getViewport(lua_State* L)
{
    auto* self = luax_checkquad(L, 1);

    auto viewport = self->getViewport();

    lua_pushnumber(L, viewport.x);
    lua_pushnumber(L, viewport.y);
    lua_pushnumber(L, viewport.w);
    lua_pushnumber(L, viewport.h);

    return 4;
}

int Wrap_Quad::getTextureDimensions(lua_State* L)
{
    auto* self = luax_checkquad(L, 1);

    int width  = self->getTextureWidth();
    int height = self->getTextureHeight();

    lua_pushnumber(L, width);
    lua_pushnumber(L, height);

    return 2;
}

int Wrap_Quad::setLayer(lua_State* L)
{
    auto* self = luax_checkquad(L, 1);
    int layer  = luaL_checkinteger(L, 2) - 1;

    self->setLayer(layer);

    return 0;
}

int Wrap_Quad::getLayer(lua_State* L)
{
    auto* self = luax_checkquad(L, 1);

    lua_pushinteger(L, self->getLayer() + 1);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "setViewport",          Wrap_Quad::setViewport          },
    { "getViewport",          Wrap_Quad::getViewport          },
    { "getTextureDimensions", Wrap_Quad::getTextureDimensions },
    { "setLayer",             Wrap_Quad::setLayer             },
    { "getLayer",             Wrap_Quad::getLayer             }
};
// clang-format on

namespace love
{
    Quad* luax_checkquad(lua_State* L, int index)
    {
        return luax_checktype<Quad>(L, index);
    }

    int open_quad(lua_State* L)
    {
        return luax_register_type(L, &Quad::type, functions);
    }
} // namespace love
