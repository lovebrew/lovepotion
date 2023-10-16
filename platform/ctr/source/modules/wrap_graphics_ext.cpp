#include <modules/graphics/wrap_graphics.hpp>
#include <modules/graphics_ext.hpp>

using namespace love;

#define instance() (Module::GetInstance<Graphics<Console::CTR>>(Module::M_GRAPHICS))

int Wrap_Graphics::Get3D(lua_State* L)
{
    luax::PushBoolean(L, instance()->Get3D());

    return 1;
}

int Wrap_Graphics::Set3D(lua_State* L)
{
    bool enabled = luax::CheckBoolean(L, 1);

    instance()->Set3D(enabled);

    return 0;
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
    { "set3D",    Wrap_Graphics::Set3D    },
    { "getDepth", Wrap_Graphics::GetDepth }
};
// clang-format on
std::span<const luaL_Reg> Wrap_Graphics::extensions = functions;
