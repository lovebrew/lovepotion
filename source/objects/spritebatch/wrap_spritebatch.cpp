#include "common/luax.h"

#include "modules/graphics/graphics.h"

#include "objects/quad/quad.h"
#include "objects/spritebatch/wrap_spritebatch.h"

using namespace love;

static inline int Add_Or_Set(lua_State* L, SpriteBatch* self, int start, int index)
{
    Quad* quad = nullptr;

    if (Luax::IsType(L, start, Quad::type))
    {
        quad = Luax::ToType<Quad>(L, start);
        start++;
    }
    else if (lua_isnil(L, start) && !lua_isnoneornil(L, start + 1))
        return Luax::TypeErrror(L, start, "Quad");

    Graphics::CheckStandardTransform(L, start, [&](const Matrix4& matrix) {
        Luax::CatchException(L, [&]() {
            if (quad)
                index = self->Add(quad, matrix, index);
            else
                index = self->Add(matrix, index);
        });
    });

    return index;
}

int Wrap_SpriteBatch::Add(lua_State* L)
{
    SpriteBatch* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);

    int index = Add_Or_Set(L, self, 2, -1);

    lua_pushinteger(L, index);

    return 1;
}

int Wrap_SpriteBatch::Set(lua_State* L)
{
    SpriteBatch* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);
    int index         = luaL_checkinteger(L, 2) - 1;

    Add_Or_Set(L, self, 3, index);

    return 0;
}

int Wrap_SpriteBatch::Clear(lua_State* L)
{
    SpriteBatch* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);

    self->Clear();

    return 0;
}

int Wrap_SpriteBatch::Flush(lua_State* L)
{
    SpriteBatch* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);

    self->Flush();

    return 0;
}

int Wrap_SpriteBatch::SetTexture(lua_State* L)
{
    SpriteBatch* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);
    Texture* texture  = Wrap_Texture::CheckTexture(L, 2);

    Luax::CatchException(L, [&]() { self->SetTexture(texture); });

    return 0;
}

int Wrap_SpriteBatch::GetTexture(lua_State* L)
{
    SpriteBatch* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);
    Texture* texture  = self->GetTexture();

    if (texture->type.IsA(Image::type))
        Luax::PushType(L, Image::type, texture);
    else if (texture->type.IsA(Canvas::type))
        Luax::PushType(L, Canvas::type, texture);
    else
        return luaL_error(L, "Unable to determine texture type.");

    return 1;
}

int Wrap_SpriteBatch::SetColor(lua_State* L)
{
    SpriteBatch* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);
    Colorf color;

    if (lua_gettop(L) <= 1)
    {
        self->SetColor();

        return 0;
    }
    else if (lua_istable(L, 2))
    {
        for (int index = 1; index <= 4; index++)
            lua_rawgeti(L, 2, index);

        color.r = luaL_checknumber(L, -4);
        color.g = luaL_checknumber(L, -3);
        color.b = luaL_checknumber(L, -2);
        color.a = luaL_optnumber(L, -1, 1.0f);

        lua_pop(L, 4);
    }
    else
    {
        color.r = (float)luaL_checknumber(L, 2);
        color.g = (float)luaL_checknumber(L, 3);
        color.b = (float)luaL_checknumber(L, 4);
        color.a = (float)luaL_optnumber(L, 5, 1.0);
    }

    self->SetColor(color);

    return 0;
}

int Wrap_SpriteBatch::GetColor(lua_State* L)
{
    SpriteBatch* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);

    bool active  = false;
    Colorf color = self->GetColor(active);

    if (!active)
        return 0;

    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);

    return 4;
}

int Wrap_SpriteBatch::GetCount(lua_State* L)
{
    SpriteBatch* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);

    lua_pushinteger(L, self->GetCount());

    return 1;
}

int Wrap_SpriteBatch::GetBufferSize(lua_State* L)
{
    SpriteBatch* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);

    lua_pushinteger(L, self->GetBufferSize());

    return 1;
}

int Wrap_SpriteBatch::SetDrawRange(lua_State* L)
{
    SpriteBatch* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);

    if (lua_isnoneornil(L, 2))
        self->SetDrawRange();
    else
    {
        int start = luaL_checkinteger(L, 2) - 1;
        int count = luaL_checkinteger(L, 3);

        Luax::CatchException(L, [&]() { self->SetDrawRange(start, count); });
    }

    return 0;
}

int Wrap_SpriteBatch::GetDrawRange(lua_State* L)
{
    SpriteBatch* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);

    int start = 0;
    int count = 1;

    if (!self->GetDrawRange(start, count))
        return 0;

    lua_pushnumber(L, start + 1);
    lua_pushnumber(L, count);

    return 2;
}

SpriteBatch* Wrap_SpriteBatch::CheckSpriteBatch(lua_State* L, int index)
{
    return Luax::CheckType<SpriteBatch>(L, 1);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "add",           Wrap_SpriteBatch::Add           },
    { "set",           Wrap_SpriteBatch::Set           },
    { "clear",         Wrap_SpriteBatch::Clear         },
    { "flush",         Wrap_SpriteBatch::Flush         },
    { "setTexture",    Wrap_SpriteBatch::SetTexture    },
    { "getTexture",    Wrap_SpriteBatch::GetTexture    },
    { "setColor",      Wrap_SpriteBatch::SetColor      },
    { "getColor",      Wrap_SpriteBatch::GetColor      },
    { "getCount",      Wrap_SpriteBatch::GetCount      },
    { "getBufferSize", Wrap_SpriteBatch::GetBufferSize },
    { "setDrawRange",  Wrap_SpriteBatch::SetDrawRange  },
    { "getDrawRange",  Wrap_SpriteBatch::GetDrawRange  },
    { 0,               0                               }
};
// clang-format on

int Wrap_SpriteBatch::Register(lua_State* L)
{
    return Luax::RegisterType(L, &SpriteBatch::type, functions, nullptr);
}
