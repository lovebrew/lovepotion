#include <objects/quad/wrap_quad.hpp>

using namespace love;

Quad* Wrap_Quad::CheckQuad(lua_State* L, int index)
{
    return luax::CheckType<Quad>(L, index);
}

int Wrap_Quad::SetViewport(lua_State* L)
{
    auto* self = Wrap_Quad::CheckQuad(L, 1);

    Quad::Viewport viewport {};

    viewport.x = luaL_checknumber(L, 2);
    viewport.y = luaL_checknumber(L, 3);
    viewport.w = luaL_checknumber(L, 4);
    viewport.h = luaL_checknumber(L, 5);

    if (lua_isnoneornil(L, 6))
        self->SetViewport(viewport);
    else
    {
        double sourceWidth  = luaL_checknumber(L, 6);
        double sourceHeight = luaL_checknumber(L, 7);

        self->Refresh(viewport, sourceWidth, sourceHeight);
    }

    return 0;
}

int Wrap_Quad::GetViewport(lua_State* L)
{
    auto* self = Wrap_Quad::CheckQuad(L, 1);

    const Quad::Viewport& viewport = self->GetViewport();

    lua_pushnumber(L, viewport.x);
    lua_pushnumber(L, viewport.y);
    lua_pushnumber(L, viewport.w);
    lua_pushnumber(L, viewport.h);

    return 4;
}

int Wrap_Quad::GetTextureDimensions(lua_State* L)
{
    auto* self = Wrap_Quad::CheckQuad(L, 1);

    lua_pushnumber(L, self->GetTextureWidth());
    lua_pushnumber(L, self->GetTextureHeight());

    return 2;
}

int Wrap_Quad::SetLayer(lua_State* L)
{
    auto* self = Wrap_Quad::CheckQuad(L, 1);

    int layer = luaL_checkinteger(L, 2) - 1;
    self->SetLayer(layer);

    return 0;
}

int Wrap_Quad::GetLayer(lua_State* L)
{
    auto* self = Wrap_Quad::CheckQuad(L, 1);

    lua_pushinteger(L, self->GetLayer() + 1);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getLayer",             Wrap_Quad::GetLayer             },
    { "getTextureDimensions", Wrap_Quad::GetTextureDimensions },
    { "getViewport",          Wrap_Quad::GetViewport          },
    { "setLayer",             Wrap_Quad::SetLayer             },
    { "setViewport",          Wrap_Quad::SetViewport          }
};
// clang-format on

int Wrap_Quad::Register(lua_State* L)
{
    return luax::RegisterType(L, &Quad::type, functions);
}
