#include "common/luax.hpp"
#include "common/version.hpp"

#include "modules/love/love.hpp"

#include "modules/data/wrap_DataModule.hpp"
#include "modules/event/wrap_Event.hpp"
#include "modules/filesystem/wrap_Filesystem.hpp"
#include "modules/timer/wrap_Timer.hpp"

#include "boot.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>

#include <stdio.h>
#include <string.h>

static constexpr char arg_lua[] = {
#include "scripts/arg.lua"
};

static constexpr char callbacks_lua[] = {
#include "scripts/callbacks.lua"
};

static constexpr char boot_lua[] = {
#include "scripts/boot.lua"
};

static constexpr char nogame_lua[] = {
#include "scripts/nogame.lua"
};

// clang-format off
static constexpr luaL_Reg modules[] =
{
    { "love.data",       Wrap_DataModule::open },
    { "love.timer",      Wrap_Timer::open      },
    { "love.event",      Wrap_Event::open      },
    { "love.filesystem", Wrap_Filesystem::open },
    { "love.arg",        love_openArg          },
    { "love.callbacks",  love_openCallbacks    },
    { "love.boot",       love_openBoot         },
    { "love.nogame",     love_openNoGame       },
    { 0,                 0                     }
};
// clang-format on

const char* love_getVersion()
{
    return __LOVE_VERSION__;
}

const char* love_getCodename()
{
    return love::CODENAME;
}

static int w_love_getVersion(lua_State* L)
{
    lua_pushinteger(L, love::LOVE_FRAMEWORK.major);
    lua_pushinteger(L, love::LOVE_FRAMEWORK.minor);
    lua_pushinteger(L, love::LOVE_FRAMEWORK.revision);

    lua_pushstring(L, love::CODENAME);

    return 4;
}

int love_isVersionCompatible(lua_State* L)
{
    love::Version check {};

    if (lua_type(L, 1) == LUA_TSTRING)
        check = love::Version(luaL_checkstring(L, 1));
    else
    {
        int major    = luaL_checkinteger(L, 1);
        int minor    = luaL_checkinteger(L, 2);
        int revision = luaL_optinteger(L, 3, 0);

        check = love::Version(major, minor, revision);
    }

    for (auto& item : love::COMPATIBILITY)
    {
        if (check != item)
            continue;

        lua_pushboolean(L, true);
        return 1;
    }

    lua_pushboolean(L, false);

    return 1;
}

static bool hasDeprecationOutput = true;
static int love_hasDeprecationOutput(lua_State* L)
{
    love::luax_pushboolean(L, hasDeprecationOutput);

    return 1;
}

static int love_setDeprecationOutput(lua_State* L)
{
    bool enable          = love::luax_checkboolean(L, 1);
    hasDeprecationOutput = enable;

    return 0;
}

static void luax_addcompatibilityalias(lua_State* L, const char* module, const char* name,
                                       const char* alias)
{
    lua_getglobal(L, module);

    if (lua_istable(L, -1))
    {
        lua_getfield(L, -1, alias);
        bool hasalias = !lua_isnoneornil(L, -1);

        lua_pop(L, 1);

        if (!hasalias)
        {
            lua_getfield(L, -1, name);
            lua_setfield(L, -2, alias);
        }
    }

    lua_pop(L, 1);
}

int love_initialize(lua_State* L)
{
    for (size_t i = 0; modules[i].name != nullptr; i++)
        love::luax_preload(L, modules[i].func, modules[i].name);

    love::luax_insistpinnedthread(L);
    love::luax_insistglobal(L, "love");

    lua_pushstring(L, __LOVE_VERSION__);
    lua_setfield(L, -2, "_version");

    lua_pushnumber(L, love::LOVE_FRAMEWORK.major);
    lua_setfield(L, -2, "_version_major");

    lua_pushnumber(L, love::LOVE_FRAMEWORK.minor);
    lua_setfield(L, -2, "_version_minor");

    lua_pushnumber(L, love::LOVE_FRAMEWORK.revision);
    lua_setfield(L, -2, "_version_revision");

    lua_pushstring(L, love::CODENAME);
    lua_setfield(L, -2, "_version_codename");

    lua_pushcfunction(L, love_openConsole);
    lua_setfield(L, -2, "_openConsole");

    lua_register(L, "print", love_print);

    lua_newtable(L);
    for (size_t index = 0; index < love::COMPATIBILITY.size(); index++)
    {
        lua_pushstring(L, love::COMPATIBILITY[index]);
        lua_rawseti(L, -2, index + 1);
    }
    lua_setfield(L, -2, "_version_compat");

    lua_pushcfunction(L, w_love_getVersion);
    lua_setfield(L, -2, "getVersion");

    lua_pushcfunction(L, love_isVersionCompatible);
    lua_setfield(L, -2, "isVersionCompatible");

    lua_pushstring(L, __OS__);
    lua_setfield(L, -2, "_os");

    lua_pushcfunction(L, love_setDeprecationOutput);
    lua_setfield(L, -2, "setDeprecationOutput");

    lua_pushcfunction(L, love_hasDeprecationOutput);
    lua_setfield(L, -2, "hasDeprecationOutput");

    love::luax_require(L, "love.data");
    lua_pop(L, 1);

#if LUA_VERSION_NUM <= 501
    luax_addcompatibilityalias(L, "math", "fmod", "mod");
    luax_addcompatibilityalias(L, "string", "gmatch", "gfind");
#endif

    // love::luasocket::preload(L);
    love::luax_preload(L, luaopen_luautf8, "utf8");
    // love::luax_preload(L, luaopen_https, "https");

    return 1;
}

/**
 * @brief Initializes the standard input/output for 3dslink.
 *
 * Users will need to use telnet on Windows or netcat on Linux/macOS:
 * `telnet/netcat 192.168.x.x 8000`
 */

#define SEND(sockfd, s) send(sockfd, s, strlen(s), 0)

int love_openConsole(lua_State* L)
{
    struct sockaddr_in server;
    int lsockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (lsockfd < 0)
    {
        love::luax_pushboolean(L, false);
        return 1;
    }

    /* make piepie happy :) */
    std::fill_n(&server, 1, sockaddr_in {});

    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = 0;
    server.sin_port        = htons(STDIO_PORT);

    if (bind(lsockfd, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        love::luax_pushboolean(L, false);
        close(lsockfd);
        return 1;
    }

    if (listen(lsockfd, 5) < 0)
    {
        love::luax_pushboolean(L, false);
        close(lsockfd);
        return 1;
    }

    int sockfd = accept(lsockfd, NULL, NULL);
    close(lsockfd);

    if (sockfd < 0)
    {
        love::luax_pushboolean(L, false);
        return 1;
    }

    // {
    //     SEND(sockfd, "HTTP/1.1 200 OK\r\n");
    //     SEND(sockfd, "Content-Type: text/plain; charset=utf-8\r\n");
    //     SEND(sockfd, "\r\n");
    // }

    std::fflush(stdout);
    dup2(sockfd, STDOUT_FILENO);

    close(sockfd);

    love::luax_pushboolean(L, true);
    return 1;
}

int love_print(lua_State* L)
{
    int argc = lua_gettop(L);
    lua_getglobal(L, "tostring");

    std::string result {};

    for (int index = 1; index <= argc; index++)
    {
        lua_pushvalue(L, -1);
        lua_pushvalue(L, index);
        lua_call(L, 1, 1);

        const char* string = lua_tostring(L, -1);
        if (string == nullptr)
            return luaL_error(L, "'tostring' must return a string to 'print'");

        if (index > 1)
            result += "\t";

        result += string;

        lua_pop(L, 1);
    }

    std::printf("[LOVE] %s\r\n", result.c_str());
    return 0;
}

int love_openNoGame(lua_State* L)
{
    if (luaL_loadbuffer(L, nogame_lua, sizeof(nogame_lua), "nogame.lua") == 0)
        lua_call(L, 0, 1);

    return 1;
}

int love_openArg(lua_State* L)
{
    if (luaL_loadbuffer(L, arg_lua, sizeof(arg_lua), "arg.lua") == 0)
        lua_call(L, 0, 1);

    return 1;
}

int love_openCallbacks(lua_State* L)
{
    if (luaL_loadbuffer(L, callbacks_lua, sizeof(callbacks_lua), "callbacks.lua") == 0)
        lua_call(L, 0, 1);

    return 1;
}

int love_openBoot(lua_State* L)
{
    if (luaL_loadbuffer(L, boot_lua, sizeof(boot_lua), "boot.lua") == 0)
        lua_call(L, 0, 1);

    return 1;
}
