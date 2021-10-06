#include "objects/text/wrap_text.h"
#include "common/luax.h"

#include "objects/transform/wrap_transform.h"

using namespace love;

int Wrap_Text::Set(lua_State* L)
{
    Text* self = Wrap_Text::CheckText(L, 1);

    std::vector<Font::ColoredString> newText;
    Wrap_Font::CheckColoredString(L, 2, newText);

    Luax::CatchException(L, [&]() { self->Set(newText); });

    return 0;
}

int Wrap_Text::Setf(lua_State* L)
{
    Text* self = Wrap_Text::CheckText(L, 1);

    float wrapLimit = (float)luaL_checknumber(L, 3);

    Font::AlignMode align;
    const char* alignStr = luaL_checkstring(L, 4);
    if (!Font::GetConstant(alignStr, align))
        return Luax::EnumError(L, "align mode", Font::GetConstants(align), alignStr);

    std::vector<Font::ColoredString> newText;
    Wrap_Font::CheckColoredString(L, 2, newText);

    Luax::CatchException(L, [&]() { self->Set(newText, wrapLimit, align); });

    return 0;
}

int Wrap_Text::Add(lua_State* L)
{
    Text* self = Wrap_Text::CheckText(L, 1);

    int index = 0;

    std::vector<Font::ColoredString> text;
    Wrap_Font::CheckColoredString(L, 2, text);

    if (Luax::IsType(L, 3, Transform::type))
    {
        Transform* transform = Luax::ToType<Transform>(L, 3);
        Luax::CatchException(L, [&]() { index = self->Add(text, transform->GetMatrix()); });
    }
    else
    {
        float x  = (float)luaL_optnumber(L, 3, 0.0);
        float y  = (float)luaL_optnumber(L, 4, 0.0);
        float a  = (float)luaL_optnumber(L, 5, 0.0);
        float sx = (float)luaL_optnumber(L, 6, 1.0);
        float sy = (float)luaL_optnumber(L, 7, sx);
        float ox = (float)luaL_optnumber(L, 8, 0.0);
        float oy = (float)luaL_optnumber(L, 9, 0.0);
        float kx = (float)luaL_optnumber(L, 10, 0.0);
        float ky = (float)luaL_optnumber(L, 11, 0.0);

        Matrix4 m(x, y, a, sx, sy, ox, oy, kx, ky);
        Luax::CatchException(L, [&]() { index = self->Add(text, m); });
    }

    lua_pushnumber(L, index + 1);

    return 1;
}

int Wrap_Text::Addf(lua_State* L)
{
    Text* self = Wrap_Text::CheckText(L, 1);

    int index = 0;

    std::vector<Font::ColoredString> text;
    Wrap_Font::CheckColoredString(L, 2, text);

    float wrap = (float)luaL_checknumber(L, 3);

    Font::AlignMode align = Font::ALIGN_MAX_ENUM;
    const char* alignStr  = luaL_checkstring(L, 4);

    if (!Font::GetConstant(alignStr, align))
        return Luax::EnumError(L, "align mode", Font::GetConstants(align), alignStr);

    if (Luax::IsType(L, 5, Transform::type))
    {
        Transform* transform = Luax::ToType<Transform>(L, 5);
        Luax::CatchException(
            L, [&]() { index = self->Addf(text, wrap, align, transform->GetMatrix()); });
    }
    else
    {
        float x  = (float)luaL_optnumber(L, 5, 0.0);
        float y  = (float)luaL_optnumber(L, 6, 0.0);
        float a  = (float)luaL_optnumber(L, 7, 0.0);
        float sx = (float)luaL_optnumber(L, 8, 1.0);
        float sy = (float)luaL_optnumber(L, 9, sx);
        float ox = (float)luaL_optnumber(L, 10, 0.0);
        float oy = (float)luaL_optnumber(L, 11, 0.0);
        float kx = (float)luaL_optnumber(L, 12, 0.0);
        float ky = (float)luaL_optnumber(L, 13, 0.0);

        Matrix4 m(x, y, a, sx, sy, ox, oy, kx, ky);
        Luax::CatchException(L, [&]() { index = self->Addf(text, wrap, align, m); });
    }

    lua_pushnumber(L, index + 1);

    return 1;
}

int Wrap_Text::Clear(lua_State* L)
{
    Text* self = Wrap_Text::CheckText(L, 1);

    Luax::CatchException(L, [&]() { self->Clear(); });

    return 0;
}

int Wrap_Text::SetFont(lua_State* L)
{
    Text* self = Wrap_Text::CheckText(L, 1);
    Font* font = Wrap_Font::CheckFont(L, 2);

    Luax::CatchException(L, [&]() { self->SetFont(font); });

    return 0;
}

int Wrap_Text::GetFont(lua_State* L)
{
    Text* self = Wrap_Text::CheckText(L, 1);
    Font* font = self->GetFont();

    Luax::PushType(L, font);

    return 1;
}

int Wrap_Text::GetWidth(lua_State* L)
{
    Text* self = Wrap_Text::CheckText(L, 1);
    int index  = luaL_optinteger(L, 2, 0) - 1;

    lua_pushnumber(L, self->GetWidth(index));

    return 1;
}

int Wrap_Text::GetHeight(lua_State* L)
{
    Text* self = Wrap_Text::CheckText(L, 1);
    int index  = luaL_optinteger(L, 2, 0) - 1;

    lua_pushnumber(L, self->GetHeight(index));

    return 1;
}

int Wrap_Text::GetDimensions(lua_State* L)
{
    Text* self = Wrap_Text::CheckText(L, 1);
    int index  = (int)luaL_optinteger(L, 2, 0) - 1;

    lua_pushnumber(L, self->GetWidth(index));
    lua_pushnumber(L, self->GetHeight(index));

    return 2;
}

Text* Wrap_Text::CheckText(lua_State* L, int index)
{
    return Luax::CheckType<Text>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "add",           Wrap_Text::Add           },
    { "addf",          Wrap_Text::Addf          },
    { "clear",         Wrap_Text::Clear         },
    { "getDimensions", Wrap_Text::GetDimensions },
    { "getFont",       Wrap_Text::GetFont       },
    { "getHeight",     Wrap_Text::GetHeight     },
    { "getWidth",      Wrap_Text::GetWidth      },
    { "set",           Wrap_Text::Set           },
    { "setf",          Wrap_Text::Setf          },
    { "setFont",       Wrap_Text::SetFont       },
    { 0,               0                        }
};
// clang-format on

int Wrap_Text::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Text::type, functions, nullptr);
}
