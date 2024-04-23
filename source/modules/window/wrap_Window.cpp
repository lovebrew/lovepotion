#include "modules/window/wrap_Window.hpp"

using namespace love;

#define instance() (Module::getInstance<Window>(Module::M_WINDOW))

int Wrap_Window::getDisplayCount(lua_State* L)
{
    lua_pushinteger(L, instance()->getDisplayCount());

    return 1;
}

int Wrap_Window::getDisplayName(lua_State* L)
{
    int displayIndex = luaL_checkinteger(L, 1) - 1;
    auto displayName = instance()->getDisplayName(displayIndex);

    luax_pushstring(L, displayName);

    return 1;
}

static int readWindowSettings(lua_State* L, int index, Window::WindowSettings& settings)
{
    luax_checktablefields<Window::Setting>(L, index, "window setting", Window::getConstant);

    lua_getfield(L, index, Window::getConstant(Window::SETTING_FULLSCREEN_TYPE));
    if (!lua_isnoneornil(L, -1))
    {
        const char* typeName = luaL_checkstring(L, -1);
        if (!Window::getConstant(typeName, settings.fsType))
            return luax_enumerror(L, "fullscreen type", Window::FullscreenTypes, typeName);
    }
    lua_pop(L, 1);

    // clang-format off
    settings.fullscreen = luax_boolflag(L, index, Window::getConstant(Window::SETTING_FULLSCREEN), settings.fullscreen);
    settings.msaa = luax_intflag(L, index, Window::getConstant(Window::SETTING_MSAA), settings.msaa);
    settings.stencil = luax_boolflag(L, index, Window::getConstant(Window::SETTING_STENCIL), settings.stencil);
    settings.resizable = luax_boolflag(L, index, Window::getConstant(Window::SETTING_RESIZABLE), settings.resizable);
    settings.minwidth = luax_intflag(L, index, Window::getConstant(Window::SETTING_MIN_WIDTH), settings.minwidth);
    settings.minheight = luax_intflag(L, index, Window::getConstant(Window::SETTING_MIN_HEIGHT), settings.minheight);
    settings.borderless = luax_boolflag(L, index, Window::getConstant(Window::SETTING_BORDERLESS), settings.borderless);
    settings.centered = luax_boolflag(L, index, Window::getConstant(Window::SETTING_CENTERED), settings.centered);
    settings.usedpiscale = luax_boolflag(L, index, Window::getConstant(Window::SETTING_USE_DPISCALE), settings.usedpiscale);
    // clang-format on

    lua_getfield(L, index, Window::getConstant(Window::SETTING_DEPTH));
    if (!lua_isnoneornil(L, -1))
        settings.depth = lua_toboolean(L, -1);
    lua_pop(L, 1);

    // clang-format off
    settings.displayindex = luax_intflag(L, index, Window::getConstant(Window::SETTING_DISPLAYINDEX), settings.displayindex);
    // clang-format on

    lua_getfield(L, index, Window::getConstant(Window::SETTING_VSYNC));
    if (lua_isnumber(L, -1))
        settings.vsync = lua_tointeger(L, -1);
    else
        settings.vsync = lua_toboolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, Window::getConstant(Window::SETTING_X));
    lua_getfield(L, index, Window::getConstant(Window::SETTING_Y));
    settings.useposition = !(lua_isnoneornil(L, -2) && lua_isnoneornil(L, -1));
    if (settings.useposition)
    {
        settings.x = lua_tointeger(L, -2);
        settings.y = lua_tointeger(L, -1);
    }
    lua_pop(L, 2);

    return 0;
}

int Wrap_Window::setMode(lua_State* L)
{
    int width  = luaL_checkinteger(L, 1);
    int height = luaL_checkinteger(L, 2);

    if (lua_isnoneornil(L, 3))
    {
        // clang-format off
        luax_catchexcept(L, [&] {
            luax_pushboolean(L, instance()->setWindow(width, height, nullptr));
        });
        // clang-format on
    }

    Window::WindowSettings settings {};
    readWindowSettings(L, 3, settings);

    // clang-format off
    luax_catchexcept(L, [&] {
        luax_pushboolean(L, instance()->setWindow(width, height, &settings));
    });
    // clang-format on

    return 1;
}

int Wrap_Window::updateMode(lua_State* L)
{
    int width, height;
    Window::WindowSettings settings {};

    instance()->getWindow(width, height, settings);

    if (lua_gettop(L) == 0)
        return luaL_error(L, "Expected at least one argument.");

    int index = 1;
    if (lua_isnumber(L, 1))
    {
        index  = 3;
        width  = luaL_checkinteger(L, 1);
        height = luaL_checkinteger(L, 2);
    }

    if (!lua_isnoneornil(L, index))
        readWindowSettings(L, index, settings);

    // clang-format off
    luax_catchexcept(L, [&] {
        luax_pushboolean(L, instance()->setWindow(width, height, &settings));
    });
    // clang-format on

    return 1;
}

int Wrap_Window::getMode(lua_State* L)
{
    int w, h;
    Window::WindowSettings settings;
    instance()->getWindow(w, h, settings);

    lua_pushnumber(L, w);
    lua_pushnumber(L, h);

    if (lua_istable(L, 1))
        lua_pushvalue(L, 1);
    else
        lua_newtable(L);

    std::string_view fstypestr = "desktop";
    Window::getConstant(settings.fsType, fstypestr);

    luax_pushstring(L, fstypestr);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_FULLSCREEN_TYPE));

    luax_pushboolean(L, settings.fullscreen);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_FULLSCREEN));

    lua_pushinteger(L, settings.vsync);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_VSYNC));

    lua_pushinteger(L, settings.msaa);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_MSAA));

    luax_pushboolean(L, settings.stencil);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_STENCIL));

    luax_pushboolean(L, settings.depth);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_DEPTH));

    luax_pushboolean(L, settings.resizable);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_RESIZABLE));

    lua_pushinteger(L, settings.minwidth);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_MIN_WIDTH));

    lua_pushinteger(L, settings.minheight);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_MIN_HEIGHT));

    luax_pushboolean(L, settings.borderless);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_BORDERLESS));

    luax_pushboolean(L, settings.centered);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_CENTERED));

    // Display index is 0-based internally and 1-based in Lua.
    lua_pushinteger(L, settings.displayindex + 1);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_DISPLAYINDEX));

    luax_pushboolean(L, settings.usedpiscale);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_USE_DPISCALE));

    lua_pushnumber(L, settings.refreshrate);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_REFRESHRATE));

    lua_pushinteger(L, settings.x);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_X));

    lua_pushinteger(L, settings.y);
    lua_setfield(L, -2, Window::getConstant(Window::SETTING_Y));

    return 3;
}

int Wrap_Window::isHighDPIAllowed(lua_State* L)
{
    lua_pushboolean(L, love::isHighDPIAllowed());

    return 1;
}

int Wrap_Window::getDisplayOrientation(lua_State* L)
{
    int displayIndex = 0;
    if (!lua_isnoneornil(L, 1))
        displayIndex = luaL_checkinteger(L, 1) - 1;

    std::string_view orientation;
    if (!Window::getConstant(instance()->getDisplayOrientation(displayIndex), orientation))
        return luax_enumerror(L, "display orientation", Window::DisplayOrientations, orientation);

    luax_pushstring(L, orientation);

    return 1;
}

int Wrap_Window::getFullscreenModes(lua_State* L)
{
    int displayIndex = 0;
    if (!lua_isnoneornil(L, 1))
        displayIndex = luaL_checkinteger(L, 1) - 1;

    const auto& modes = instance()->getFullscreenSizes(displayIndex);
    lua_createtable(L, modes.size(), 0);

    for (size_t index = 0; index < modes.size(); index++)
    {
        lua_pushinteger(L, index + 1);
        lua_createtable(L, 0, 2);

        lua_pushinteger(L, modes[index].width);
        lua_setfield(L, -2, "width");

        lua_pushinteger(L, modes[index].height);
        lua_setfield(L, -2, "height");

        lua_settable(L, -3);
    }

    return 1;
}

int Wrap_Window::getFullscreen(lua_State* L)
{
    bool fullscreen = luax_checkboolean(L, 1);
    auto type       = Window::FullscreenType::FULLSCREEN_MAX_ENUM;

    const char* typeName = lua_isnoneornil(L, 2) ? nullptr : luaL_checkstring(L, 2);
    if (typeName && !Window::getConstant(typeName, type))
        return luax_enumerror(L, "fullscreen type", Window::FullscreenTypes, typeName);

    bool success = false;
    luax_catchexcept(L, [&] {
        if (type == Window::FullscreenType::FULLSCREEN_MAX_ENUM)
            success = instance()->setFullscreen(fullscreen);
        else
            success = instance()->setFullscreen(fullscreen, type);
    });

    luax_pushboolean(L, success);

    return 1;
}

int Wrap_Window::isOpen(lua_State* L)
{
    lua_pushboolean(L, instance()->isOpen());

    return 1;
}

int Wrap_Window::close(lua_State* L)
{
    instance()->close();

    return 0;
}

int Wrap_Window::getDesktopDimensions(lua_State* L)
{
    int displayIndex = luaL_checkinteger(L, 1) - 1;
    int width, height;

    instance()->getDesktopDimensions(displayIndex, width, height);

    lua_pushinteger(L, width);
    lua_pushinteger(L, height);

    return 2;
}

int Wrap_Window::setVSync(lua_State* L)
{
    return 0;
}

int Wrap_Window::getVSync(lua_State* L)
{
    lua_pushnil(L);

    return 1;
}

int Wrap_Window::setDisplaySleepEnabled(lua_State* L)
{
    bool enable = luax_checkboolean(L, 1);

    instance()->setDisplaySleepEnabled(enable);

    return 0;
}

int Wrap_Window::isDisplaySleepEnabled(lua_State* L)
{
    lua_pushboolean(L, instance()->isDisplaySleepEnabled());

    return 1;
}

int Wrap_Window::hasFocus(lua_State* L)
{
    luax_pushboolean(L, true);

    return 1;
}

int Wrap_Window::isVisible(lua_State* L)
{
    luax_pushboolean(L, true);

    return 1;
}

int Wrap_Window::getDPIScale(lua_State* L)
{
    lua_pushnumber(L, instance()->getDPIScale());

    return 1;
}

int Wrap_Window::getNativeDPIScale(lua_State* L)
{
    lua_pushnumber(L, instance()->getNativeDPIScale());

    return 1;
}

int Wrap_Window::toPixels(lua_State* L)
{
    double x = luaL_checknumber(L, 1);

    if (lua_isnoneornil(L, 2))
    {
        lua_pushnumber(L, instance()->toPixels(x));
        return 1;
    }

    double y      = luaL_checknumber(L, 2);
    double pixelx = 0.0, pixely = 0.0;

    instance()->toPixels(x, y, pixelx, pixely);

    lua_pushnumber(L, pixelx);
    lua_pushnumber(L, pixely);

    return 2;
}

int Wrap_Window::fromPixels(lua_State* L)
{
    double pixelx = luaL_checknumber(L, 1);

    if (lua_isnoneornil(L, 2))
    {
        lua_pushnumber(L, instance()->fromPixels(pixelx));
        return 1;
    }

    double pixely = luaL_checknumber(L, 2);
    double x = 0.0, y = 0.0;

    instance()->fromPixels(pixelx, pixely, x, y);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    return 2;
}

int Wrap_Window::isMaximized(lua_State* L)
{
    luax_pushboolean(L, true);

    return 1;
}

int Wrap_Window::isMinimized(lua_State* L)
{
    luax_pushboolean(L, false);

    return 1;
}

// TODO: Call an internal function in Lua from C++ with our data
int Wrap_Window::showMessageBox(lua_State* L)
{
    Window::MessageBoxData data {};
    data.type = Window::MESSAGEBOX_INFO;

    data.title   = luaL_checkstring(L, 1);
    data.message = luaL_checkstring(L, 2);

    if (lua_istable(L, 3))
    {
        size_t numButtons = std::clamp<size_t>(luax_objlen(L, 3), 0, 2);

        if (numButtons == 0)
            return luaL_error(L, "Must have at least one messagebox button.");

        for (size_t index = 0; index < numButtons; index++)
        {
            lua_rawgeti(L, 3, index + 1);
            data.buttons.push_back(luaL_checkstring(L, -1));
            lua_pop(L, 1);
        }

        lua_getfield(L, 3, "enterbutton");
        if (!lua_isnoneornil(L, -1))
            data.enterButtonIndex = luaL_checkinteger(L, -1) - 1;
        else
            data.enterButtonIndex = 0;
        lua_pop(L, 1);

        lua_getfield(L, 3, "escapebutton");
        if (!lua_isnoneornil(L, -1))
            data.escapeButtonIndex = luaL_checkinteger(L, -1) - 1;
        else
            data.escapeButtonIndex = 0;
        lua_pop(L, 1);

        const char* typeName = lua_isnoneornil(L, 4) ? nullptr : luaL_checkstring(L, 4);
        if (typeName && !Window::getConstant(typeName, data.type))
            return luax_enumerror(L, "messagebox type", Window::MessageBoxTypes, typeName);

        data.attachToWindow = luax_optboolean(L, 5, true);

        lua_pushnil(L);
    }
    else
    {
        const char* typeName = lua_isnoneornil(L, 3) ? nullptr : luaL_checkstring(L, 3);
        if (typeName && !Window::getConstant(typeName, data.type))
            return luax_enumerror(L, "messagebox type", Window::MessageBoxTypes, typeName);

        data.attachToWindow = luax_optboolean(L, 4, true);

        lua_pushnil(L);
    }

    return 1;
}

int Wrap_Window::setTitle(lua_State* L)
{
    return 0;
}

int Wrap_Window::setIcon(lua_State* L)
{
    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getDisplayCount",         Wrap_Window::getDisplayCount        },
    { "getDisplayName",          Wrap_Window::getDisplayName         },
    { "setMode",                 Wrap_Window::setMode                },
    { "updateMode",              Wrap_Window::updateMode             },
    { "getMode",                 Wrap_Window::getMode                },
    { "isHighDPIAllowed",        Wrap_Window::isHighDPIAllowed       },
    { "getDisplayOrientation",   Wrap_Window::getDisplayOrientation  },
    { "getFullscreenModes",      Wrap_Window::getFullscreenModes     },
    { "getFullscreen",           Wrap_Window::getFullscreen          },
    { "isOpen",                  Wrap_Window::isOpen                 },
    { "close",                   Wrap_Window::close                  },
    { "getDesktopDimensions",    Wrap_Window::getDesktopDimensions   },
    { "setVSync",                Wrap_Window::setVSync               },
    { "getVSync",                Wrap_Window::getVSync               },
    { "setDisplaySleepEnabled",  Wrap_Window::setDisplaySleepEnabled },
    { "isDisplaySleepEnabled",   Wrap_Window::isDisplaySleepEnabled  },
    { "hasFocus",                Wrap_Window::hasFocus               },
    { "isVisible",               Wrap_Window::isVisible              },
    { "getDPIScale",             Wrap_Window::getDPIScale            },
    { "getNativeDPIScale",       Wrap_Window::getNativeDPIScale      },
    { "toPixels",                Wrap_Window::toPixels               },
    { "fromPixels",              Wrap_Window::fromPixels             },
    { "isMaximized",             Wrap_Window::isMaximized            },
    { "isMinimized",             Wrap_Window::isMinimized            },
    { "showMessageBox",          Wrap_Window::showMessageBox         },
    { "setTitle",                Wrap_Window::setTitle               },
    { "setIcon",                 Wrap_Window::setIcon                }
};
// clang-format on

int Wrap_Window::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&]() { instance = new Window(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "window";
    module.type      = &Module::type;
    module.functions = functions;

    return luax_register_module(L, module);
}
