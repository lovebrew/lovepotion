#include "objects/font/wrap_font.h"
#include "modules/graphics/graphics.h"

#include "common/lmath.h"

using namespace love;

int Wrap_Font::GetWidth(lua_State* L)
{
    love::Font* self = Wrap_Font::CheckFont(L, 1);
    const char* text = luaL_checkstring(L, 2);

    Luax::CatchException(L, [&]() { lua_pushinteger(L, self->GetWidth(text)); });

    return 1;
}

int Wrap_Font::GetHeight(lua_State* L)
{
    love::Font* self = Wrap_Font::CheckFont(L, 1);

    float height = self->GetHeight();

    lua_pushnumber(L, height);

    return 1;
}

int Wrap_Font::GetWrap(lua_State* L)
{
    love::Font* self = Wrap_Font::CheckFont(L, 1);

    std::vector<Font::ColoredString> text;
    Wrap_Font::CheckColoredString(L, 2, text);

    float wrap = luaL_checknumber(L, 3);

    int maxWidth = 0;
    std::vector<std::string> lines;
    std::vector<int> widths;

    Luax::CatchException(L, [&]() { self->GetWrap(text, wrap, lines, &widths); });

    for (int width : widths)
        maxWidth = std::max(maxWidth, width);

    lua_pushinteger(L, maxWidth);
    lua_createtable(L, lines.size(), 0);

    for (size_t index = 0; index < lines.size(); index++)
    {
        lua_pushstring(L, lines[index].c_str());
        lua_rawseti(L, -2, index + 1);
    }

    return 2;
}

/*
** NOTE: this does not affect the 3DS
** version of LÖVE Potion
*/
int Wrap_Font::SetLineHeight(lua_State* L)
{
    love::Font* self = Wrap_Font::CheckFont(L, 1);
    float height     = luaL_checknumber(L, 2);

    self->SetLineHeight(height);

    return 0;
}

int Wrap_Font::GetLineHeight(lua_State* L)
{
    love::Font* self = Wrap_Font::CheckFont(L, 1);

    lua_pushnumber(L, self->GetLineHeight());

    return 1;
}

int Wrap_Font::SetFilter(lua_State* L)
{
    love::Font* self       = Wrap_Font::CheckFont(L, 1);
    Texture::Filter filter = self->GetFilter();

    const char* min = luaL_checkstring(L, 2);
    const char* mag = luaL_optstring(L, 3, min);

    if (!Texture::GetConstant(min, filter.min))
        return Luax::EnumError(L, "filter mode", Texture::GetConstants(filter.min), min);

    if (!Texture::GetConstant(mag, filter.mag))
        return Luax::EnumError(L, "filter mode", Texture::GetConstants(filter.mag), mag);

    filter.anisotropy = luaL_optnumber(L, 4, 1.0f);

    Luax::CatchException(L, [&]() { self->SetFilter(filter); });

    return 0;
}

int Wrap_Font::GetFilter(lua_State* L)
{
    love::Font* self = Wrap_Font::CheckFont(L, 1);

    const Texture::Filter filter = self->GetFilter();

    const char* min;
    const char* mag;

    Texture::GetConstant(filter.min, min);
    Texture::GetConstant(filter.mag, mag);

    lua_pushstring(L, min);
    lua_pushstring(L, mag);

    lua_pushnumber(L, filter.anisotropy);

    return 3;
}

int Wrap_Font::GetAscent(lua_State* L)
{
    love::Font* self = Wrap_Font::CheckFont(L, 1);

    lua_pushnumber(L, self->GetAscent());

    return 1;
}

int Wrap_Font::GetDescent(lua_State* L)
{
    love::Font* self = Wrap_Font::CheckFont(L, 1);

    lua_pushnumber(L, self->GetDescent());

    return 1;
}

int Wrap_Font::GetBaseline(lua_State* L)
{
    love::Font* self = Wrap_Font::CheckFont(L, 1);

    lua_pushnumber(L, self->GetBaseline());

    return 1;
}

int Wrap_Font::HasGlyphs(lua_State* L)
{
    love::Font* self = Wrap_Font::CheckFont(L, 1);
    bool hasGlyph    = false;

    size_t count = std::max(lua_gettop(L) - 1, 1);

    Luax::CatchException(L, [&]() {
        for (size_t index = 2; index < count + 2; index++)
        {
            if (lua_type(L, index) == LUA_TSTRING)
                hasGlyph = self->HasGlyphs(Luax::CheckString(L, index));
            else
                hasGlyph = self->HasGlyph((uint32_t)luaL_checknumber(L, index));

            if (!hasGlyph)
                break;
        }
    });

    lua_pushboolean(L, hasGlyph);

    return 1;
}

/*
** Note: 3DS has zero kerning
** So this will always return zero anyways.
*/
int Wrap_Font::GetKerning(lua_State* L)
{
    love::Font* self = Wrap_Font::CheckFont(L, 1);

    float kerning = 0.0f;

    Luax::CatchException(L, [&]() {
        if (lua_type(L, 2) == LUA_TSTRING)
        {
            std::string left  = Luax::CheckString(L, 2);
            std::string right = Luax::CheckString(L, 3);

            kerning = self->GetKerning(left, right);
        }
        else
        {
            uint32_t left  = (uint32_t)luaL_checknumber(L, 2);
            uint32_t right = (uint32_t)luaL_checknumber(L, 3);
            kerning        = self->GetKerning(left, right);
        }
    });

    lua_pushnumber(L, kerning);

    return 1;
}

int Wrap_Font::SetFallbacks(lua_State* L)
{
    love::Font* self = Wrap_Font::CheckFont(L, 1);
    std::vector<love::Font*> fallbacks;

    size_t count = lua_gettop(L);

    for (size_t index = 2; index <= count; index++)
        fallbacks.push_back(Wrap_Font::CheckFont(L, index));

    Luax::CatchException(L, [&]() { self->SetFallbacks(fallbacks); });

    return 0;
}

int Wrap_Font::GetDPIScale(lua_State* L)
{
    love::Font* self = Wrap_Font::CheckFont(L, 1);

    lua_pushnumber(L, self->GetDPIScale());

    return 1;
}

Font* Wrap_Font::CheckFont(lua_State* L, int index)
{
    return Luax::CheckType<Font>(L, index);
}

void Wrap_Font::CheckColoredString(lua_State* L, int index,
                                   std::vector<Font::ColoredString>& strings)
{
    Font::ColoredString coloredString;
    coloredString.color = Colorf(1.0f, 1.0f, 1.0f, 1.0f);

    if (lua_istable(L, index))
    {
        int length = (int)lua_objlen(L, index);

        for (int i = 1; i <= length; i++)
        {
            lua_rawgeti(L, index, i);

            if (lua_istable(L, -1))
            {
                for (int j = 1; j <= 4; j++)
                    lua_rawgeti(L, -j, j);

                coloredString.color.r = luaL_checknumber(L, -4);
                coloredString.color.g = luaL_checknumber(L, -3);
                coloredString.color.b = luaL_checknumber(L, -2);
                coloredString.color.a = luaL_optnumber(L, -1, 1.0f);

                lua_pop(L, 4);
            }
            else
            {
                coloredString.string = luaL_checkstring(L, -1);
                strings.push_back(coloredString);
            }

            lua_pop(L, 1);
        }
    }
    else
    {
        coloredString.string = luaL_checkstring(L, index);
        strings.push_back(coloredString);
    }
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getHeight",     Wrap_Font::GetHeight     },
    { "getWidth",      Wrap_Font::GetWidth      },
    { "getWrap",       Wrap_Font::GetWrap       },
    { "setLineHeight", Wrap_Font::SetLineHeight },
    { "getAscent",     Wrap_Font::GetAscent     },
    { "getBaseline",   Wrap_Font::GetBaseline   },
    { "getDescent",    Wrap_Font::GetDescent    },
    { "getDPIScale",   Wrap_Font::GetDPIScale   },
    { "getFilter",     Wrap_Font::GetFilter     },
    { "getKerning",    Wrap_Font::GetKerning    },
    { "getLineHeight", Wrap_Font::GetLineHeight },
    { "hasGlyphs",     Wrap_Font::HasGlyphs     },
    { "setFallbacks",  Wrap_Font::SetFallbacks  },
    { "setFilter",     Wrap_Font::SetFilter     },
    { 0,               0                        }
};
// clang-format on

int Wrap_Font::Register(lua_State* L)
{
    return Luax::RegisterType(L, &love::Font::type, functions, nullptr);
}
