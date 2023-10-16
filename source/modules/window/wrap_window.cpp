#include <modules/window/wrap_window.hpp>
#include <modules/window_ext.hpp>

static constexpr char wrap_window_lua[] = {
#include "scripts/wrap_window.lua"
};

static constexpr char wrap_window_messagebox_lua[] = {
#include "scripts/wrap_window_messagebox.lua"
};

using namespace love;

#define instance() (Module::GetInstance<Window<Console::Which>>(Module::M_WINDOW))

int Wrap_Window::SetMode(lua_State* L)
{
    int width  = luaL_checkinteger(L, 1);
    int height = luaL_checkinteger(L, 2);

    Window<>::WindowSettings settings {};

    /* force to the console size on switch */
    instance()->GetWindow(width, height, settings);

    if (lua_isnoneornil(L, 3))
    {
        // clang-format off
        luax::CatchException(L, [&]() {
            luax::PushBoolean(L, instance()->SetWindow(width, height));
        });
        // clang-format on

        return 1;
    }

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

static int loadMessagebox(lua_State* L)
{
    if (luaL_loadbuffer(L, (const char*)wrap_window_messagebox_lua,
                        sizeof(wrap_window_messagebox_lua), "wrap_window_messagebox.lua") == 0)
    {
        lua_call(L, 0, 1);
    }
    else
        lua_error(L);

    return 1;
}

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

    int result = luax::RegisterModule(L, wrappedModule);

    luax::Preload(L, loadMessagebox, "wrap_window_messagebox");

    if (luaL_loadbuffer(L, (const char*)wrap_window_lua, sizeof(wrap_window_lua),
                        "wrap_window.lua") == 0)
    {
        lua_call(L, 0, 0);
    }
    else
        lua_error(L);

    return result;
}
