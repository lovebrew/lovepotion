#include <objects/font/wrap_font.hpp>

using namespace love;

int Wrap_Font::GetWrap(lua_State* L)
{
    auto* self = Wrap_Font::CheckFont(L, 1);

    std::vector<ColoredString> text;
    Wrap_Font::CheckColoredString(L, 2, text);

    float wrap = luaL_checknumber(L, 3);

    int maxWidth = 0;
    std::vector<std::string> lines;
    std::vector<int> widths;

    luax::CatchException(L, [&]() { self->GetWrap(text, wrap, lines, &widths); });

    for (const auto width : widths)
        maxWidth = std::max(maxWidth, width);

    lua_pushinteger(L, maxWidth);

    lua_createtable(L, (int)lines.size(), 0);
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
    { "getWrap", Wrap_Font::GetWrap }
};
// clang-format on

std::span<const luaL_Reg> Wrap_Font::extensions = functions;
