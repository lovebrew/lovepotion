#include "common/luax.hpp"
#include "common/version.hpp"

#include <luasocket.hpp>

#include "modules/love/love.hpp"

#include "modules/audio/wrap_Audio.hpp"
#include "modules/data/wrap_DataModule.hpp"
#include "modules/event/wrap_Event.hpp"
#include "modules/filesystem/wrap_Filesystem.hpp"
#include "modules/font/wrap_Font.hpp"
#include "modules/graphics/wrap_Graphics.hpp"
#include "modules/image/wrap_Image.hpp"
#include "modules/joystick/wrap_JoystickModule.hpp"
#include "modules/keyboard/wrap_Keyboard.hpp"
#include "modules/math/wrap_MathModule.hpp"
#include "modules/sensor/wrap_Sensor.hpp"
#include "modules/sound/wrap_Sound.hpp"
#include "modules/system/wrap_System.hpp"
#include "modules/thread/wrap_Thread.hpp"
#include "modules/timer/wrap_Timer.hpp"
#include "modules/touch/wrap_Touch.hpp"
#include "modules/window/wrap_Window.hpp"

#include "boot.hpp"

// #region DEBUG CONSOLE
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/socket.h>

#if defined(__WIIU__)
    #include <unistd.h>
#endif
// #endregion

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
static constexpr std::array<const luaL_Reg, 21> modules =
{{
    { "love.audio",      Wrap_Audio::open          },
    { "love.data",       Wrap_DataModule::open     },
    { "love.event",      Wrap_Event::open          },
    { "love.filesystem", Wrap_Filesystem::open     },
    { "love.font",       Wrap_FontModule::open     },
    { "love.graphics",   Wrap_Graphics::open       },
    { "love.joystick",   Wrap_JoystickModule::open },
    { "love.image",      Wrap_Image::open          },
    { "love.keyboard",   Wrap_Keyboard::open       },
    { "love.math",       Wrap_MathModule::open     },
    { "love.sensor",     Wrap_Sensor::open         },
    { "love.sound",      Wrap_Sound::open          },
    { "love.system",     Wrap_System::open         },
    { "love.thread",     Wrap_Thread::open         },
    { "love.timer",      Wrap_Timer::open          },
    { "love.touch",      Wrap_Touch::open          },
    { "love.window",     Wrap_Window::open         },
    { "love.nogame",     love_openNoGame           },
    { "love.arg",        love_openArg              },
    { "love.callbacks",  love_openCallbacks        },
    { "love.boot",       love_openBoot             }
}};
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

#if defined(__3DS__)
static bool hasOSSpeedup = false;
/*
** Sets the New Nintendo 3DS "OS Speedup"
** This does nothing™ for Old 3DS systems.
*/
static int love_setOSSpeedup(lua_State* L)
{
    const auto enable = love::luax_checkboolean(L, 1);
    osSetSpeedupEnable(enable);

    hasOSSpeedup = enable;

    return 0;
}

static int love_hasOSSpeedup(lua_State* L)
{
    love::luax_pushboolean(L, hasOSSpeedup);

    return 1;
}
#endif

static constexpr const char* ERROR_PANIC = "PANIC: unprotected error in call to Lua API (%s)";

/*
 * If an error happens outside any protected environment, Lua calls a panic function and then calls
 * exit(EXIT_FAILURE), thus exiting the host application. We want to inform the user of the error.
 * However, this will only be informative via the console.
 */
static int love_atpanic(lua_State* L)
{
    char message[0x80] {};
    snprintf(message, sizeof(message), ERROR_PANIC, lua_tostring(L, -1));

    printf("%s\n", message);
    return 0;
}

#if __DEBUG__
static void love_atcpanic()
{
    try
    {
        throw;
    }
    catch (const std::exception& e)
    {
        std::printf("Uncaught exception: %s\n", e.what());
        std::exit(EXIT_FAILURE);
    }
}
#endif

static void luax_addcompatibilityalias(lua_State* L, const char* module, const char* name, const char* alias)
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
    for (auto& module : modules)
        love::luax_preload(L, module.func, module.name);

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

    lua_pushstring(L, __CONSOLE__);
    lua_setfield(L, -2, "_console");

    lua_pushcfunction(L, love_setDeprecationOutput);
    lua_setfield(L, -2, "setDeprecationOutput");

    lua_pushcfunction(L, love_hasDeprecationOutput);
    lua_setfield(L, -2, "hasDeprecationOutput");

#if defined(__3DS__)
    lua_pushcfunction(L, love_setOSSpeedup);
    lua_setfield(L, -2, "_setOSSpeedup");

    lua_pushcfunction(L, love_hasOSSpeedup);
    lua_setfield(L, -2, "_hasOSSpeedup");
#endif

    love::luax_require(L, "love.data");
    lua_pop(L, 1);

#if LUA_VERSION_NUM <= 501
    luax_addcompatibilityalias(L, "math", "fmod", "mod");
    luax_addcompatibilityalias(L, "string", "gmatch", "gfind");
#endif

    love::luasocket::preload(L);
    love::luax_preload(L, luaopen_luautf8, "utf8");
    love::luax_preload(L, luaopen_https, "https");

    lua_atpanic(L, love_atpanic);

#if __DEBUG__
    std::set_terminate(love_atcpanic);
#endif

    return 1;
}

/**
 * @brief Initializes the console output.
 *
 * Users will need to use telnet on Windows or netcat on Linux/macOS:
 * `telnet/nc 192.168.x.x 8000`
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

    fd_set set;
    FD_ZERO(&set);
    FD_SET(lsockfd, &set);

    struct timeval timeout;
    timeout.tv_sec  = 3;
    timeout.tv_usec = 0;

    const auto result = select(lsockfd + 1, &set, NULL, NULL, &timeout);

    if (result == -1)
    {
        // select(...) failed
        love::luax_pushboolean(L, false);
        close(lsockfd);
        return 1;
    }
    else if (result == 0)
    {
        // timeout occurred
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
