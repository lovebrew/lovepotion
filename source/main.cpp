extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>

    #include <compat-5.3.h>
    #include <luaobj.h>

    #include <lutf8lib.h>
}

#if defined (_3DS)
    #include <3ds.h>
    bool appletMainLoop() {
        return aptMainLoop();
    }
#elif defined (__SWITCH__)
    #include <switch.h>
#endif

#include <string>

#include "common/logger.h"
#include "modules/timer.h"

#include <stdio.h>
#include <stdarg.h>
#include <map>
#include <vector>

#include "common/types.h"
#include "common/util.h"

#include "socket/luasocket.h"
#include "modules/filesystem.h"
#include "modules/love.h"

#include "boot_lua.h"

#define luaL_dobuffer(L, b, n, s) \
    (luaL_loadbuffer(L, b, n, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

int main(int argc, char * argv[])
{
    lua_State * L = luaL_newstate();

    luaL_openlibs(L);

    love_preload(L, luaopen_luautf8, "utf8");
    love_preload(L, LuaSocket::InitSocket, "socket");
    love_preload(L, LuaSocket::InitHTTP,   "socket.http");

    //Logger::Initialize();

    char * path = (argc == 2) ? argv[1] : argv[0];
    Filesystem::Initialize(path);

    luaL_requiref(L, "love", Love::Initialize, true);
    lua_pop(L, -1); // don't leave the love module on the stack

    Love::InitConstants(L);

    luaL_dobuffer(L, (char *)boot_lua, boot_lua_size, "boot");

    while (appletMainLoop())
    {
        if (Love::IsRunning())
            luaL_dostring(L, "xpcall(love.run, love.errorhandler)");
        else
            break;
    }

    Love::Exit(L);

    //Logger::Exit();

    return 0;
}
