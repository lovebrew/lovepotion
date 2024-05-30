#include "modules/graphics/wrap_Font.hpp"

#include <algorithm>

using namespace love;

int Wrap_Font::getWidth(lua_State* L)
{
    auto* self = luax_checkfont(L, 1);

    if (lua_type(L, 2) == LUA_TSTRING)
    {
        auto* string = luaL_checkstring(L, 2);
        luax_catchexcept(L, [&] { lua_pushnumber(L, self->getWidth(string)); });
    }
    else
    {
        auto glyph = (uint32_t)luaL_checknumber(L, 2);
        luax_catchexcept(L, [&] { lua_pushnumber(L, self->getWidth(glyph)); });
    }

    return 1;
}

int Wrap_Font::getHeight(lua_State* L)
{
    auto* self = luax_checkfont(L, 1);

    lua_pushnumber(L, self->getHeight());

    return 1;
}

int Wrap_Font::getWrap(lua_State* L)
{
    auto* self = luax_checkfont(L, 1);

    std::vector<ColoredString> text {};
    luax_checkcoloredstring(L, 2, text);

    float wrap = (float)luaL_checknumber(L, 3);

    int maxWidth = 0;
    std::vector<std::string> lines {};
    std::vector<int> widths {};

    luax_catchexcept(L, [&]() { self->getWrap(text, wrap, lines, &widths); });

    for (int width : widths)
        maxWidth = std::max(maxWidth, width);

    lua_pushinteger(L, maxWidth);
    lua_createtable(L, (int)lines.size(), 0);

    for (int i = 0; i < (int)lines.size(); i++)
    {
        lua_pushstring(L, lines[i].c_str());
        lua_rawseti(L, -2, i + 1);
    }

    return 2;
}

int Wrap_Font::setLineHeight(lua_State* L)
{
    auto* self   = luax_checkfont(L, 1);
    float height = (float)luaL_checknumber(L, 2);

    self->setLineHeight(height);

    return 0;
}

int Wrap_Font::getLineHeight(lua_State* L)
{
    auto* self = luax_checkfont(L, 1);

    lua_pushnumber(L, self->getLineHeight());

    return 1;
}

int Wrap_Font::setFilter(lua_State* L)
{
    auto* self        = luax_checkfont(L, 1);
    auto samplerState = self->getSamplerState();

    const char* minString = luaL_checkstring(L, 2);
    const char* magString = luaL_optstring(L, 3, minString);

    if (!SamplerState::getConstant(minString, samplerState.minFilter))
        return luax_enumerror(L, "filter mode", SamplerState::FilterModes, minString);

    if (!SamplerState::getConstant(magString, samplerState.magFilter))
        return luax_enumerror(L, "filter mode", SamplerState::FilterModes, magString);

    samplerState.maxAnisotropy = std::min(std::max(1, (int)luaL_optnumber(L, 4, 1.0)), LOVE_UINT8_MAX);

    luax_catchexcept(L, [&]() { self->setSamplerState(samplerState); });

    return 0;
}

int Wrap_Font::getFilter(lua_State* L)
{
    auto* self        = luax_checkfont(L, 1);
    auto samplerState = self->getSamplerState();

    std::string_view minString {};
    std::string_view magString {};

    SamplerState::getConstant(samplerState.minFilter, minString);
    SamplerState::getConstant(samplerState.magFilter, magString);

    luax_pushstring(L, minString);
    luax_pushstring(L, magString);
    lua_pushnumber(L, samplerState.maxAnisotropy);

    return 3;
}

int Wrap_Font::getAscent(lua_State* L)
{
    auto* self = luax_checkfont(L, 1);

    lua_pushnumber(L, self->getAscent());

    return 1;
}

int Wrap_Font::getDescent(lua_State* L)
{
    auto* self = luax_checkfont(L, 1);

    lua_pushnumber(L, self->getDescent());

    return 1;
}

int Wrap_Font::getBaseline(lua_State* L)
{
    auto* self = luax_checkfont(L, 1);

    lua_pushnumber(L, self->getBaseline());

    return 1;
}

int Wrap_Font::hasGlyphs(lua_State* L)
{
    auto* self = luax_checkfont(L, 1);

    bool hasGlyph = false;
    int count     = std::max(lua_gettop(L) - 1, 1);

    luax_catchexcept(L, [&]() {
        for (int index = 2; index < count; index++)
        {
            if (lua_type(L, index) == LUA_TSTRING)
                hasGlyph = self->hasGlyphs(luax_checkstring(L, index));
            else
                hasGlyph = self->hasGlyph((uint32_t)luaL_checknumber(L, index));

            if (!hasGlyph)
                break;
        }
    });

    luax_pushboolean(L, hasGlyph);

    return 1;
}

int Wrap_Font::getKerning(lua_State* L)
{
    auto* self    = luax_checkfont(L, 1);
    float kerning = 0.0f;

    luax_catchexcept(L, [&]() {
        if (lua_type(L, 2) == LUA_TSTRING)
        {
            auto left  = luax_checkstring(L, 2);
            auto right = luax_checkstring(L, 3);

            kerning = self->getKerning(left, right);
        }
        else
        {
            auto left  = (uint32_t)luaL_checknumber(L, 2);
            auto right = (uint32_t)luaL_checknumber(L, 3);

            kerning = self->getKerning(left, right);
        }
    });

    lua_pushnumber(L, kerning);

    return 1;
}

int Wrap_Font::setFallbacks(lua_State* L)
{
    auto* self = luax_checkfont(L, 1);

    std::vector<FontBase*> fallbacks {};

    for (int index = 2; index <= lua_gettop(L); index++)
        fallbacks.push_back(luax_checkfont(L, index));

    luax_catchexcept(L, [&]() { self->setFallbacks(fallbacks); });

    return 0;
}

int Wrap_Font::getDPIScale(lua_State* L)
{
    auto* self = luax_checkfont(L, 1);

    lua_pushnumber(L, self->getDPIScale());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getWidth",      Wrap_Font::getWidth      },
    { "getHeight",     Wrap_Font::getHeight     },
    { "getWrap",       Wrap_Font::getWrap       },
    { "setLineHeight", Wrap_Font::setLineHeight },
    { "getLineHeight", Wrap_Font::getLineHeight },
    { "setFilter",     Wrap_Font::setFilter     },
    { "getFilter",     Wrap_Font::getFilter     },
    { "getAscent",     Wrap_Font::getAscent     },
    { "getDescent",    Wrap_Font::getDescent    },
    { "getBaseline",   Wrap_Font::getBaseline   },
    { "hasGlyphs",     Wrap_Font::hasGlyphs     },
    { "setFallbacks",  Wrap_Font::setFallbacks  },
    { "getDPIScale",   Wrap_Font::getDPIScale   }
};
// clang-format on

namespace love
{
    FontBase* luax_checkfont(lua_State* L, int index)
    {
        return luax_checktype<FontBase>(L, index);
    }

    void luax_checkcoloredstring(lua_State* L, int index, std::vector<ColoredString>& strings)
    {
        ColoredString coloredString {};
        coloredString.color = Color(1.0f, 1.0f, 1.0f, 1.0f);

        if (lua_istable(L, index))
        {
            const int length = luax_objlen(L, index);

            for (int i = 0; i <= length; i++)
            {
                lua_rawgeti(L, index, i + 1);

                if (lua_istable(L, -1))
                {
                    for (int j = 1; j <= 4; j++)
                        lua_rawgeti(L, -j, j);

                    coloredString.color.r = (float)luaL_checknumber(L, -4);
                    coloredString.color.g = (float)luaL_checknumber(L, -3);
                    coloredString.color.b = (float)luaL_checknumber(L, -2);
                    coloredString.color.a = (float)luaL_optnumber(L, -1, 1.0f);

                    lua_pop(L, 4);
                }
                else
                {
                    coloredString.string = luaL_checkstring(L, -1);
                    strings.push_back(coloredString);
                }
            }
        }
        else
        {
            coloredString.string = luaL_checkstring(L, index);
            strings.push_back(coloredString);
        }
    }

    int open_font(lua_State* L)
    {
        luax_register_type(L, &FontBase::type, functions);

        return 0;
    }
} // namespace love
