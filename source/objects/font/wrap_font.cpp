#include <objects/font/wrap_font.hpp>

using namespace love;

void Wrap_Font::CheckColoredString(lua_State* L, int index, ColoredStrings& strings)
{
    ColoredString coloredString {};
    coloredString.color = Color { 1.0f, 1.0f, 1.0f, 1.0f };

    if (lua_istable(L, index))
    {
        const auto length = luax::ObjectLength(L, index);

        for (size_t stackpos = 1; stackpos <= length; stackpos++)
        {
            lua_rawgeti(L, index, stackpos);

            if (lua_istable(L, -1))
            {
                for (int colorIndex = 1; colorIndex <= 4; colorIndex++)
                    lua_rawgeti(L, -colorIndex, colorIndex);

                coloredString.color.r = luaL_checknumber(L, -4);
                coloredString.color.g = luaL_checknumber(L, -3);
                coloredString.color.b = luaL_checknumber(L, -2);
                coloredString.color.a = luaL_optnumber(L, -1, 1.0f);

                lua_pop(L, 4);
            }
            else
            {
                coloredString.str = luaL_checkstring(L, -1);
                strings.push_back(coloredString);
            }

            lua_pop(L, 1);
        }
    }
    else
    {
        coloredString.str = luaL_checkstring(L, index);
        strings.push_back(coloredString);
    }
}

Font* Wrap_Font::CheckFont(lua_State* L, int index)
{
    return luax::CheckType<Font>(L, index);
}

int Wrap_Font::GetWidth(lua_State* L)
{
    auto* self = Wrap_Font::CheckFont(L, 1);

    if (lua_type(L, 2) == LUA_TSTRING)
    {
        const char* string = luaL_checkstring(L, 2);
        luax::CatchException(L, [&]() { lua_pushinteger(L, self->GetWidth(string)); });
    }
    else
    {
        uint32_t glyph = luaL_checknumber(L, 2);
        luax::CatchException(L, [&]() { lua_pushinteger(L, self->GetWidth(glyph)); });
    }

    return 1;
}

int Wrap_Font::GetHeight(lua_State* L)
{
    auto* self = Wrap_Font::CheckFont(L, 1);

    lua_pushnumber(L, self->GetHeight());

    return 1;
}

int Wrap_Font::SetLineHeight(lua_State* L)
{
    auto* self   = Wrap_Font::CheckFont(L, 1);
    float height = luaL_checknumber(L, 2);

    self->SetLineHeight(height);

    return 0;
}

int Wrap_Font::GetLineHeight(lua_State* L)
{
    auto* self = Wrap_Font::CheckFont(L, 1);

    lua_pushnumber(L, self->GetLineHeight());

    return 1;
}

int Wrap_Font::SetFilter(lua_State* L)
{
    auto* self         = Wrap_Font::CheckFont(L, 1);
    SamplerState state = self->GetSamplerState();

    const char* min = luaL_checkstring(L, 2);
    const char* mag = luaL_optstring(L, 3, min);

    std::optional<SamplerState::FilterMode> minOpt;
    std::optional<SamplerState::FilterMode> magOpt;

    if (!(minOpt = SamplerState::filterModes.Find(min)))
        return luax::EnumError(L, "filter mode", SamplerState::filterModes, min);

    if (!(magOpt = SamplerState::filterModes.Find(mag)))
        return luax::EnumError(L, "filter mode", SamplerState::filterModes, mag);

    int maxAnisotropy   = luaL_optnumber(L, 4, 1.0f);
    state.maxAnisotropy = std::clamp<int>(maxAnisotropy, 1, std::numeric_limits<uint8_t>::max());

    luax::CatchException(L, [&]() { self->SetSamplerState(state); });

    return 0;
}

int Wrap_Font::GetFilter(lua_State* L)
{
    auto* self                = Wrap_Font::CheckFont(L, 1);
    const SamplerState& state = self->GetSamplerState();

    std::optional<const char*> min;
    std::optional<const char*> mag;

    min = SamplerState::filterModes.ReverseFind(state.minFilter);
    mag = SamplerState::filterModes.ReverseFind(state.magFilter);

    lua_pushstring(L, *min);
    lua_pushstring(L, *mag);

    lua_pushnumber(L, state.maxAnisotropy);

    return 3;
}

int Wrap_Font::GetAscent(lua_State* L)
{
    auto* self = Wrap_Font::CheckFont(L, 1);

    lua_pushnumber(L, self->GetAscent());

    return 1;
}

int Wrap_Font::GetDescent(lua_State* L)
{
    auto* self = Wrap_Font::CheckFont(L, 1);

    lua_pushnumber(L, self->GetDPIScale());

    return 1;
}

int Wrap_Font::GetBaseline(lua_State* L)
{
    auto* self = Wrap_Font::CheckFont(L, 1);

    lua_pushnumber(L, self->GetBaseline());

    return 1;
}

int Wrap_Font::HasGlyphs(lua_State* L)
{
    auto* self    = Wrap_Font::CheckFont(L, 1);
    bool hasGlyph = false;

    const auto count = std::max(lua_gettop(L) - 1, 1);

    luax::CatchException(L, [&]() {
        for (int index = 2; index < count; index++)
        {
            if (lua_type(L, index) == LUA_TSTRING)
                hasGlyph = self->HasGlyphs(luax::CheckString(L, index));
            else
                hasGlyph = self->HasGlyph(luaL_checknumber(L, index));

            if (!hasGlyph)
                break;
        }
    });

    luax::PushBoolean(L, hasGlyph);

    return 1;
}

int Wrap_Font::GetKerning(lua_State* L)
{
    auto* self    = Wrap_Font::CheckFont(L, 1);
    float kerning = 0.0f;

    luax::CatchException(L, [&]() {
        if (lua_type(L, 2) == LUA_TSTRING)
        {
            std::string left  = luax::CheckString(L, 2);
            std::string right = luax::CheckString(L, 3);

            kerning = self->GetKerning(left, right);
        }
        else
        {
            uint32_t left  = luaL_checknumber(L, 2);
            uint32_t right = luaL_checknumber(L, 3);

            kerning = self->GetKerning(left, right);
        }
    });

    lua_pushnumber(L, kerning);

    return 1;
}

int Wrap_Font::SetFallbacks(lua_State* L)
{
    auto* self = Wrap_Font::CheckFont(L, 1);
    std::vector<Font*> fallbacks;

    for (int index = 2; index <= lua_gettop(L); index++)
        fallbacks.push_back(Wrap_Font::CheckFont(L, index));

    luax::CatchException(L, [&]() { self->SetFallbacks(fallbacks); });

    return 0;
}

int Wrap_Font::GetDPIScale(lua_State* L)
{
    auto* self = Wrap_Font::CheckFont(L, 1);

    lua_pushnumber(L, self->GetDPIScale());

    return 1;
}

int Wrap_Font::GetWrap(lua_State* L)
{
    auto* self = Wrap_Font::CheckFont(L, 1);

    std::vector<ColoredString> text {};
    Wrap_Font::CheckColoredString(L, 2, text);

    float wrap = luaL_checknumber(L, 3);

    int maxWidth = 0;

    std::vector<std::string> lines {};
    std::vector<int> widths {};

    luax::CatchException(L, [&]() { self->GetWrap(text, wrap, lines, &widths); });

    for (const auto width : widths)
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

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getWidth",      Wrap_Font::GetWidth      },
    { "getHeight",     Wrap_Font::GetHeight     },
    { "setLineHeight", Wrap_Font::SetLineHeight },
    { "getLineHeight", Wrap_Font::GetLineHeight },
    { "setFilter",     Wrap_Font::SetFilter     },
    { "getFilter",     Wrap_Font::GetFilter     },
    { "getAscent",     Wrap_Font::GetAscent     },
    { "getDescent",    Wrap_Font::GetDescent    },
    { "getBaseline",   Wrap_Font::GetBaseline   },
    { "hasGlyphs",     Wrap_Font::HasGlyphs     },
    { "getKerning",    Wrap_Font::GetKerning    },
    { "setFallbacks",  Wrap_Font::SetFallbacks  },
    { "getDPIScale",   Wrap_Font::GetDPIScale   },
    { "getWrap",       Wrap_Font::GetWrap       }
};
// clang-format on

int Wrap_Font::Register(lua_State* L)
{
    return luax::RegisterType(L, &Font::type, functions);
}
