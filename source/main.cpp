extern "C" 
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>

    #include <compat-5.2.h>
    #include <luaobj.h>
}

#include <string>

#include <SDL.h>

#include "modules/audio.h"
#include "modules/filesystem.h"
#include "modules/graphics.h"
#include "modules/window.h"
#include "modules/system.h"

#include "modules/timer.h"

#include <stdio.h>
#include <stdarg.h>
#include <map>
#include <vector>

#include <switch.h>

#include "objects/object/object.h"
#include "objects/gamepad/gamepad.h"
#include "objects/gamepad/wrap_gamepad.h"

#include "socket/luasocket.h"

#include "modules/love.h"
#include "modules/joystick.h"

#include "boot_lua.h"

#include "common/console.h"
#include "common/types.h"
#include "common/variant.h"
#include "common/util.h"
#include "common/version.h"

bool ERROR = false;
bool LOVE_QUIT = false;

int main(int argc, char * argv[])
{
    lua_State * L = luaL_newstate();

    luaL_openlibs(L);

    love_preload(L, LuaSocket::InitSocket, "socket");
    love_preload(L, LuaSocket::InitHTTP, "socket.http");

    luaL_requiref(L, "love", Love::Initialize, 1);

    Love::InitModules(L);

    lua_getglobal(L, "love");

    lua_newtable(L);
    lua_pushnumber(L, 1);
    lua_pushstring(L,"Horizon");
    lua_pushnumber(L, 2);
    lua_pushstring(L,"Switch");
    lua_rawset(L, -3);
    lua_setfield(L, -2, "_os");

    lua_pushstring(L, Love::VERSION);
    lua_setfield(L, -2, "_version");
    lua_pushnumber(L, Love::VERSION_MAJOR);
    lua_setfield(L, -2, "_version_major");
    lua_pushnumber(L, Love::VERSION_MINOR);
    lua_setfield(L, -2, "_version_minor");
    lua_pushnumber(L, Love::VERSION_REVISION);
    lua_setfield(L, -2, "_version_revision");
    lua_pushstring(L, -2, Love::CODENAME);
    lua_setfield(L, -2, "_version_codename");

    luaL_dobuffer(L, (char *)boot_lua, boot_lua_size, "boot");

    /*
    ** aptMainLoop important code moved to love.cpp
    ** this was to register it as love.run
    ** for error handling purposes
    */
    
    while (appletMainLoop())
    {
        if (Love::IsRunning())
            luaL_dostring(L, "xpcall(love.run, love.errhand)");
        else
            break;
    }

    LuaSocket::Close();

    Love::Exit(L);

    return 0;
}
