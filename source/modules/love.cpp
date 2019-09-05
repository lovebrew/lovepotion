#include "common/runtime.h"

#include "common/version.h"
#include "common/types.h"
#include "common/util.h"

#include "objects/gamepad/gamepad.h"

#include "modules/event.h"
#include "modules/graphics.h"
#include "modules/joystick.h"
#include "modules/timer.h"
#include "modules/window.h"
#include "modules/display.h"

#include "socket/luasocket.h"

#include "nogame_lua.h"

void Love::InitModules(lua_State * L)
{
    for (int i = 0; modules[i].name; i++)
    {
        if (modules[i].init)
            modules[i].init();
    }
}

void Love::InitConstants(lua_State * L)
{
    lua_getglobal(L, "love");

    // love._constants
    // love._os = "Horizon"
    lua_newtable(L);
    lua_pushnumber(L, 1);
    lua_pushstring(L, "Horizon");
    lua_rawset(L, -3);
    lua_setfield(L, -2, "_os");

    //love._consolename
    lua_pushstring(L, LOVE_POTION_CONSOLE);
    lua_setfield(L, -2, "_console_name");

    // love._version stuff
    lua_pushstring(L, Version::VERSION.c_str());
    lua_setfield(L, -2, "_version");

    // major
    lua_pushnumber(L, Version::VERSION_MAJOR);
    lua_setfield(L, -2, "_version_major");
    lua_pushnumber(L, Version::VERSION_MINOR);
    lua_setfield(L, -2, "_version_minor");
    lua_pushnumber(L, Version::VERSION_REVISION);
    lua_setfield(L, -2, "_version_revision");
    lua_pushstring(L, Version::CODENAME.c_str());
    lua_setfield(L, -2, "_version_codename");

    lua_pop(L, 1);
}

int Love::Initialize(lua_State * L)
{
    for (int i = 0; classes[i]; i++)
    {
        classes[i](L);
        lua_pop(L, 1);
    }

    luaL_Reg reg[] =
    {
        { "_nogame",       NoGame     },
        { "getVersion",    GetVersion },
        { "run",           Run        },
        { "quit",          Quit       },
        { 0, 0 }
    };

    luaL_newlib(L, reg);

    // preload all the modules
    char modname[30];
    for (int i = 0; modules[i].name; i++)
    {
        strcpy(modname, "love.");
        strcat(modname, modules[i].name);
        love_preload(L, modules[i].reg, modname);
    }

    Display::Initialize();

    LuaSocket::Initialize();

    love_get_registry(L, OBJECTS);
    if (!lua_istable(L, -1))
    {
        lua_newtable(L);
        lua_replace(L, -2);

        lua_setfield(L, LUA_REGISTRYINDEX, "_loveobjects");
    }

    state = L;

    return 1;
}

int Love::RaiseError(const char * format, ...)
{
    char buffer[256];

    va_list args;

    va_start(args, format);

    vsprintf(buffer, format, args);

    va_end(args);

    return luaL_error(state, buffer, "");
}

bool Love::IsRunning()
{
    return quit == false;
}

int Love::Quit(lua_State * L)
{
    quit = true;

    return 0;
}

//love.getVersion
int Love::GetVersion(lua_State * L)
{
    if (!lua_isnoneornil(L, 1))
    {
        if (lua_type(L, 1) == LUA_TBOOLEAN && lua_toboolean(L, 1))
        {
            lua_pushstring(L, LOVE_POTION_VERSION);

            return 1;
        }
    }

    lua_pushnumber(L, Version::VERSION_MAJOR);
    lua_pushnumber(L, Version::VERSION_MINOR);
    lua_pushnumber(L, Version::VERSION_REVISION);
    lua_pushstring(L, Version::CODENAME.c_str());

    return 4;
}

int Love::NoGame(lua_State * L)
{
    luaL_dobuffer(L, (char *)nogame_lua, nogame_lua_size, "nogame");

    return 0;
}

void Love::Exit(lua_State * L)
{
    for (int i = 0; modules[i].name; i++)
    {
        if (modules[i].close)
            modules[i].close();
    }

    LuaSocket::Exit();

    lua_close(L);

    Display::Exit();
    Graphics::Exit();
}
