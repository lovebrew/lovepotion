#include <objects/spritebatch/wrap_spritebatch.hpp>

#include <modules/graphics/wrap_graphics.hpp>

#include <objects/texture/wrap_texture.hpp>

using namespace love;

SpriteBatch* Wrap_SpriteBatch::CheckSpriteBatch(lua_State* L, int index)
{
    return luax::CheckType<SpriteBatch>(L, 1);
}

static inline int addOrSet(lua_State* L, SpriteBatch* self, int start, int index)
{
    Quad* quad = nullptr;

    if (luax::IsType(L, start, Quad::type))
    {
        quad = luax::ToType<Quad>(L, start);
        start++;
    }
    else if (lua_isnil(L, start) && !lua_isnoneornil(L, start + 1))
        return luax::TypeError(L, start, "Quad");

    Wrap_Graphics::CheckStandardTransform(L, start, [&](const Matrix4<Console::Which>& matrix) {
        luax::CatchException(L, [&]() {
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
    auto* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);
    int index  = addOrSet(L, self, 2, -1);

    lua_pushinteger(L, index + 1);
    return 1;
}

int Wrap_SpriteBatch::Set(lua_State* L)
{
    auto* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);
    int index  = luaL_checkinteger(L, 2) - 1;

    addOrSet(L, self, 3, index);

    return 0;
}

int Wrap_SpriteBatch::GetTexture(lua_State* L)
{
    auto* self          = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);
    const auto* texture = self->GetTexture();

    luax::PushType(L, texture);

    return 1;
}

int Wrap_SpriteBatch::SetTexture(lua_State* L)
{
    auto* self    = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);
    auto* texture = Wrap_Texture::CheckTexture(L, 2);

    self->SetTexture(texture);

    return 0;
}

int Wrap_SpriteBatch::GetColor(lua_State* L)
{
    auto* self       = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);
    const auto color = self->GetColor();

    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);

    return 4;
}

int Wrap_SpriteBatch::SetColor(lua_State* L)
{
    auto* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);
    Color color {};

    if (lua_istable(L, 2))
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
        color.r = luaL_checknumber(L, 2);
        color.g = luaL_checknumber(L, 3);
        color.b = luaL_checknumber(L, 4);
        color.a = luaL_optnumber(L, 5, 1.0f);
    }

    self->SetColor(color);

    return 0;
}

int Wrap_SpriteBatch::SetDrawRange(lua_State* L)
{
    auto* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);

    if (lua_isnoneornil(L, 2))
        self->SetDrawRange();
    else
    {
        int start = luaL_checkinteger(L, 2) - 1;
        int count = luaL_checkinteger(L, 3);

        luax::CatchException(L, [&]() { self->SetDrawRange(start, count); });
    }

    return 0;
}

int Wrap_SpriteBatch::GetDrawRange(lua_State* L)
{
    auto* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);

    int start = 0;
    int count = 1;
    if (!self->GetDrawRange(start, count))
        return 0;

    lua_pushnumber(L, start + 1);
    lua_pushnumber(L, count);

    return 2;
}

int Wrap_SpriteBatch::Clear(lua_State* L)
{
    auto* self = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);
    self->Clear();

    return 0;
}

int Wrap_SpriteBatch::Flush(lua_State* L)
{
    return 0;
}

int Wrap_SpriteBatch::GetBufferSize(lua_State* L)
{
    auto* self      = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);
    const auto size = self->GetBufferSize();

    lua_pushinteger(L, size);

    return 1;
}

int Wrap_SpriteBatch::GetCount(lua_State* L)
{
    auto* self       = Wrap_SpriteBatch::CheckSpriteBatch(L, 1);
    const auto count = self->GetCount();

    lua_pushinteger(L, count);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] = 
{
    { "add",           Wrap_SpriteBatch::Add           },
    { "clear",         Wrap_SpriteBatch::Clear         },
    { "flush",         Wrap_SpriteBatch::Flush         },
    { "getBufferSize", Wrap_SpriteBatch::GetBufferSize },
    { "getColor",      Wrap_SpriteBatch::GetColor      },
    { "getCount",      Wrap_SpriteBatch::GetCount      },
    { "getDrawRange",  Wrap_SpriteBatch::GetDrawRange  },
    { "getTexture",    Wrap_SpriteBatch::GetTexture    },
    { "set",           Wrap_SpriteBatch::Set           },
    { "setColor",      Wrap_SpriteBatch::SetColor      },
    { "setDrawRange",  Wrap_SpriteBatch::SetDrawRange  },
    { "setTexture",    Wrap_SpriteBatch::SetTexture    },
    { 0,               0                               }
};
// clang-format on

int Wrap_SpriteBatch::Register(lua_State* L)
{
    return luax::RegisterType(L, &SpriteBatch::type, functions);
}