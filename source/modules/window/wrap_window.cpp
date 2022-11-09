#include <modules/window/wrap_window.hpp>
#include <modules/window_ext.hpp>

using namespace love;

#define instance() (Module::GetInstance<Window<Console::Which>>(Module::M_WINDOW))

int Wrap_Window::SetMode(lua_State* L)
{
    int width  = luaL_checkinteger(L, 1);
    int height = luaL_checkinteger(L, 2);

    if (lua_isnoneornil(L, 3))
    {
        // clang-format off
        luax::CatchException(L, [&]() {
            luax::PushBoolean(L, instance()->SetWindow(width, height));
        });
        // clang-format on

        return 1;
    }

    Window<>::WindowSettings settings {};
    // readWindowSettings(L, 3, settings) -- unneeded

    // clang-format off
    luax::CatchException(L, [&]() {
        luax::PushBoolean(L, instance()->SetWindow(width, height, &settings));
     });
    // clang-format on

    return 1;
}

int Wrap_Window::IsOpen(lua_State* L)
{
    luax::PushBoolean(L, instance()->IsOpen());

    return 1;
}

int Wrap_Window::SetTitle(lua_State* L)
{
    return 0;
}

int Wrap_Window::SetIcon(lua_State* L)
{
    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "isOpen",   Wrap_Window::IsOpen   },
    { "setIcon",  Wrap_Window::SetIcon  },
    { "setMode",  Wrap_Window::SetMode  },
    { "setTitle", Wrap_Window::SetTitle }
};
// clang-format on

int Wrap_Window::Register(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new Window<Console::Which>(); });
    else
        instance()->Retain();

    WrappedModule wrappedModule {};
    wrappedModule.instance  = instance;
    wrappedModule.name      = "window";
    wrappedModule.type      = &Module::type;
    wrappedModule.functions = functions;
    wrappedModule.types     = nullptr;

    return luax::RegisterModule(L, wrappedModule);
}
