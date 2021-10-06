#include "modules/window/wrap_window.h"

#include "window_button_lua.h"
#include "window_messagebox_lua.h"
#include "wrap_window_lua.h"

using namespace love;

#define instance() (Module::GetInstance<Window>(Module::M_WINDOW))

int Wrap_Window::GetDisplayCount(lua_State* L)
{
    lua_pushinteger(L, instance()->GetDisplayCount());

    return 1;
}

int Wrap_Window::GetFullscreenModes(lua_State* L)
{
    unsigned int display = luaL_optnumber(L, 1, 1);
    auto displaySizes    = instance()->GetFullscreenModes();

    if (!display || display > displaySizes.size())
    {
        lua_newtable(L);

        return 1;
    }

    display = std::clamp((int)display - 1, 0, (int)displaySizes.size() - 1);

    lua_createtable(L, 1, 0);
    lua_pushnumber(L, 1);

    lua_createtable(L, 0, 2);

    // Inner table attributes
    lua_pushnumber(L, displaySizes[display].first);
    lua_setfield(L, -2, "width");

    lua_pushnumber(L, displaySizes[display].second);
    lua_setfield(L, -2, "height");
    // End table attributes

    lua_settable(L, -3);

    return 1;
}

int Wrap_Window::IsOpen(lua_State* L)
{
    lua_pushboolean(L, instance()->IsOpen());

    return 1;
}

int Wrap_Window::SetMode(lua_State* L)
{
    Luax::CatchException(L, [&]() { lua_pushboolean(L, instance()->SetMode()); });

    return 1;
}

int Wrap_Window::LoadButtons(lua_State* L)
{
    if (luaL_loadbuffer(L, (const char*)window_button_lua, window_button_lua_size,
                        "window_button.lua") == 0)
        lua_call(L, 0, 1);
    else
        lua_error(L);

    return 1;
}

int Wrap_Window::LoadMessageBox(lua_State* L)
{
    if (luaL_loadbuffer(L, (const char*)window_messagebox_lua, window_messagebox_lua_size,
                        "window_messagebox.lua") == 0)
        lua_call(L, 0, 1);
    else
        lua_error(L);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getDisplayCount",    Wrap_Window::GetDisplayCount    },
    { "getFullscreenModes", Wrap_Window::GetFullscreenModes },
    { "isOpen",             Wrap_Window::IsOpen             },
    { "setMode",            Wrap_Window::SetMode            },
    { 0,                    0                               }
};
// clang-format on

int Wrap_Window::Register(lua_State* L)
{
    Window* instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Window(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "window";
    wrappedModule.type      = &Module::type;
    wrappedModule.functions = functions;
    wrappedModule.types     = nullptr;

    int ret = Luax::RegisterModule(L, wrappedModule);

    Luax::Preload(L, Wrap_Window::LoadButtons, "window_button");
    Luax::Preload(L, Wrap_Window::LoadMessageBox, "window_messagebox");

    /* load window buffer */
    if (luaL_loadbuffer(L, (const char*)wrap_window_lua, wrap_window_lua_size, "wrap_window.lua") ==
        0)
        lua_call(L, 0, 0);
    else
        lua_error(L);

    return ret;
}
