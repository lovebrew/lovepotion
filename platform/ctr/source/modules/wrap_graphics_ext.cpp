#include <modules/graphics/wrap_graphics.hpp>
#include <utilities/driver/renderer_ext.hpp>

using Renderer = love::Renderer<love::Console::CTR>;

int Wrap_Graphics::Get3D(lua_State* L)
{
    luax::PushBoolean(L, ::Renderer::Instance().Get3D());

    return 1;
}

int Wrap_Graphics::GetDepth(lua_State* L)
{
    lua_pushnumber(L, osGet3DSliderState());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "get3D",    Wrap_Graphics::Get3D    },
    { "getDepth", Wrap_Graphics::GetDepth }
};
// clang-format on
std::span<const luaL_Reg> Wrap_Graphics::extensions = functions;
