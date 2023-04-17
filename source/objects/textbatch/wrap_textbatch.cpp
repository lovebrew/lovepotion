#include <objects/textbatch/wrap_textbatch.hpp>

using TextBatch = love::TextBatch<love::Console::Which>;
using namespace love;

::TextBatch* Wrap_TextBatch::CheckTextBatch(lua_State* L, int index)
{
    return luax::CheckType<::TextBatch>(L, index);
}

int Wrap_TextBatch::Set(lua_State* L)
{
    auto* self = Wrap_TextBatch::CheckTextBatch(L, 1);

    Font<>::ColoredStrings newText {};
    Wrap_Font::CheckColoredString(L, 2, newText);

    luax::CatchException(L, [&]() { self->Set(newText); });

    return 0;
}

int Wrap_TextBatch::Setf(lua_State* L)
{
    auto* self  = Wrap_TextBatch::CheckTextBatch(L, 1);
    float limit = luaL_checknumber(L, 3);

    std::optional<Font<>::AlignMode> align;
    const char* alignMode = luaL_checkstring(L, 4);

    if (!(align = Font<>::alignModes.Find(alignMode)))
        return luax::EnumError(L, "align mode", Font<>::alignModes, alignMode);

    Font<>::ColoredStrings newText {};
    Wrap_Font::CheckColoredString(L, 2, newText);

    luax::CatchException(L, [&]() { self->Set(newText, limit, *align); });

    return 0;
}

int Wrap_TextBatch::Add(lua_State* L)
{
    auto* self = Wrap_TextBatch::CheckTextBatch(L, 1);

    int index = 0;
    Font<>::ColoredStrings text {};
    Wrap_Font::CheckColoredString(L, 2, text);

    if (luax::IsType(L, 3, Transform::type))
    {
        Transform* transform = luax::ToType<Transform>(L, 3);
        luax::CatchException(L, [&]() { index = self->Add(text, transform->GetMatrix()); });
    }
    else
    {
        float x  = luaL_optnumber(L, 3, 0.0);
        float y  = luaL_optnumber(L, 4, 0.0);
        float a  = luaL_optnumber(L, 5, 0.0);
        float sx = luaL_optnumber(L, 6, 1.0);
        float sy = luaL_optnumber(L, 7, sx);
        float ox = luaL_optnumber(L, 8, 0.0);
        float oy = luaL_optnumber(L, 9, 0.0);
        float kx = luaL_optnumber(L, 10, 0.0);
        float ky = luaL_optnumber(L, 11, 0.0);

        Matrix4<Console::Which> matrix(x, y, a, sx, sy, ox, oy, kx, ky);
        luax::CatchException(L, [&]() { index = self->Add(text, matrix); });
    }

    lua_pushnumber(L, index + 1);

    return 1;
}

int Wrap_TextBatch::Addf(lua_State* L)
{
    auto* self = Wrap_TextBatch::CheckTextBatch(L, 1);

    int index = 0;

    Font<>::ColoredStrings text {};
    Wrap_Font::CheckColoredString(L, 2, text);

    float wrap = luaL_checknumber(L, 3);

    std::optional<Font<>::AlignMode> align;
    const char* alignName = luaL_checkstring(L, 4);

    if (!(align = Font<>::alignModes.Find(alignName)))
        return luax::EnumError(L, "align mode", Font<>::alignModes, alignName);

    if (luax::IsType(L, 5, Transform::type))
    {
        Transform* transform = luax::ToType<Transform>(L, 5);
        luax::CatchException(
            L, [&]() { index = self->Addf(text, wrap, *align, transform->GetMatrix()); });
    }
    else
    {
        float x  = luaL_optnumber(L, 5, 0.0);
        float y  = luaL_optnumber(L, 6, 0.0);
        float a  = luaL_optnumber(L, 7, 0.0);
        float sx = luaL_optnumber(L, 8, 1.0);
        float sy = luaL_optnumber(L, 9, sx);
        float ox = luaL_optnumber(L, 10, 0.0);
        float oy = luaL_optnumber(L, 11, 0.0);
        float kx = luaL_optnumber(L, 12, 0.0);
        float ky = luaL_optnumber(L, 13, 0.0);

        Matrix4<Console::Which> matrix(x, y, a, sx, sy, ox, oy, kx, ky);
        luax::CatchException(L, [&]() { index = self->Addf(text, wrap, *align, matrix); });
    }

    lua_pushnumber(L, index + 1);

    return 1;
}

int Wrap_TextBatch::Clear(lua_State* L)
{
    auto* self = Wrap_TextBatch::CheckTextBatch(L, 1);

    luax::CatchException(L, [&]() { self->Clear(); });

    return 0;
}

int Wrap_TextBatch::SetFont(lua_State* L)
{
    auto* self = Wrap_TextBatch::CheckTextBatch(L, 1);
    auto* font = luax::CheckType<Font<Console::Which>>(L, 2);

    luax::CatchException(L, [&]() { self->SetFont(font); });

    return 0;
}

int Wrap_TextBatch::GetFont(lua_State* L)
{
    auto* self = Wrap_TextBatch::CheckTextBatch(L, 1);
    auto* font = self->GetFont();

    luax::PushType(L, font);

    return 1;
}

int Wrap_TextBatch::GetWidth(lua_State* L)
{
    auto* self = Wrap_TextBatch::CheckTextBatch(L, 1);
    int index  = luaL_optinteger(L, 2, 0) - 1;

    lua_pushnumber(L, self->GetWidth(index));

    return 1;
}

int Wrap_TextBatch::GetHeight(lua_State* L)
{
    auto* self = Wrap_TextBatch::CheckTextBatch(L, 1);
    int index  = luaL_optinteger(L, 2, 0) - 1;

    lua_pushnumber(L, self->GetHeight(index));

    return 1;
}

int Wrap_TextBatch::GetDimensions(lua_State* L)
{
    auto* self = Wrap_TextBatch::CheckTextBatch(L, 1);
    int index  = luaL_optinteger(L, 2, 0) - 1;

    lua_pushnumber(L, self->GetWidth(index));
    lua_pushnumber(L, self->GetHeight(index));

    return 2;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "add",           Wrap_TextBatch::Add           },
    { "addf",          Wrap_TextBatch::Addf          },
    { "clear",         Wrap_TextBatch::Clear         },
    { "getDimensions", Wrap_TextBatch::GetDimensions },
    { "getFont",       Wrap_TextBatch::GetFont       },
    { "getHeight",     Wrap_TextBatch::GetHeight     },
    { "getWidth",      Wrap_TextBatch::GetWidth      },
    { "set",           Wrap_TextBatch::Set           },
    { "setf",          Wrap_TextBatch::Setf          },
    { "setFont",       Wrap_TextBatch::SetFont       }
};
// clang-format on

int Wrap_TextBatch::Register(lua_State* L)
{
    return luax::RegisterType(L, &::TextBatch::type, functions);
}
