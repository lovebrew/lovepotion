#include "objects/quad/wrap_quad.h"

using namespace love;

int Wrap_Quad::GetTextureDimensions(lua_State* L)
{
    Quad* self = Wrap_Quad::CheckQuad(L, 1);

    lua_pushnumber(L, self->GetTextureWidth());
    lua_pushnumber(L, self->GetTextureHeight());

    return 2;
}

int Wrap_Quad::GetViewport(lua_State* L)
{
    Quad* self = Wrap_Quad::CheckQuad(L, 1);

    Quad::Viewport v = self->GetViewport();

    lua_pushnumber(L, v.x);
    lua_pushnumber(L, v.y);

    lua_pushnumber(L, v.w);
    lua_pushnumber(L, v.h);

    return 4;
}

int Wrap_Quad::SetViewport(lua_State* L)
{
    Quad* self = Wrap_Quad::CheckQuad(L, 1);

    Quad::Viewport v;

    v.x = luaL_checknumber(L, 2);
    v.y = luaL_checknumber(L, 3);

    v.w = luaL_checknumber(L, 4);
    v.h = luaL_checknumber(L, 5);

    if (lua_isnoneornil(L, 6))
        self->SetViewport(v);
    else
    {
        double sw = luaL_checknumber(L, 6);
        double sh = luaL_checknumber(L, 7);

        self->Refresh(v, sw, sh);
    }

    return 1;
}

Quad* Wrap_Quad::CheckQuad(lua_State* L, int index)
{
    return Luax::CheckType<Quad>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getTextureDimensions", Wrap_Quad::GetTextureDimensions },
    { "getViewport",          Wrap_Quad::GetViewport          },
    { "setViewport",          Wrap_Quad::SetViewport          },
    { 0,                      0                               }
};
// clang-format on

int Wrap_Quad::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Quad::type, functions, nullptr);
}
