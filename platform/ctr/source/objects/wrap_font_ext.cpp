#include <objects/font/wrap_font.hpp>

static constexpr char wrap_font_lua[] = {
#include <scripts/wrap_font.lua>
};

using namespace love;

// clang-format off
std::function<void(lua_State* L)> Wrap_Font::wrap_extension = [](lua_State* L)
{
    luax::WrapObject(L, wrap_font_lua, sizeof(wrap_font_lua), "=[love \"wrap_font.lua\"]",
                     Font::type);
};
// clang-format on
