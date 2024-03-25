#include <common/luax.hpp>
#include <common/version.hpp>

#include <luasocket.hpp>

#include <modules/love/love.hpp>

#include <algorithm>

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
#include <scripts/nogame.lua>
};

#include <modules/graphics/graphics.tcc>

#include <modules/audio/wrap_audio.hpp>
#include <modules/data/wrap_data.hpp>
#include <modules/event/wrap_event.hpp>
#include <modules/filesystem/wrap_filesystem.hpp>
#include <modules/font/wrap_fontmodule.hpp>
#include <modules/graphics/wrap_graphics.hpp>
#include <modules/image/wrap_imagemodule.hpp>
#include <modules/joystick/wrap_joystickmodule.hpp>
#include <modules/keyboard/wrap_keyboard.hpp>
#include <modules/math/wrap_math.hpp>
#include <modules/physics/wrap_physics.hpp>
#include <modules/sensor/wrap_sensor.hpp>
#include <modules/sound/wrap_sound.hpp>
#include <modules/system/wrap_system.hpp>
#include <modules/thread/wrap_threadmodule.hpp>
#include <modules/timer/wrap_timer.hpp>
#include <modules/touch/wrap_touch.hpp>
#include <modules/window/wrap_window.hpp>

#include <arpa/inet.h>

// clang-format off
static constexpr luaL_Reg modules[] =
{
    { "love.audio",      Wrap_Audio::Register          },
    { "love.data",       Wrap_DataModule::Register     },
    { "love.event",      Wrap_Event::Register          },
    { "love.filesystem", Wrap_Filesystem::Register     },
    { "love.font",       Wrap_FontModule::Register     },
    { "love.graphics",   Wrap_Graphics::Register       },
    { "love.joystick",   Wrap_JoystickModule::Register },
    { "love.keyboard",   Wrap_Keyboard::Register       },
    { "love.math",       Wrap_Math::Register           },
    { "love.image",      Wrap_ImageModule::Register    },
    { "love.physics",    Wrap_Physics::Register        },
    { "love.sensor",     Wrap_Sensor::Register         },
    { "love.sound",      Wrap_Sound::Register          },
    { "love.system",     Wrap_System::Register         },
    { "love.thread",     Wrap_ThreadModule::Register   },
    { "love.timer",      Wrap_Timer::Register          },
    { "love.touch",      Wrap_Touch::Register          },
    { "love.window",     Wrap_Window::Register         },
    { "love.arg",        love::LoadArgs                },
    { "love.callbacks",  love::LoadCallbacks           },
    { "love.boot",       love::Boot                    },
    { "love.nogame",     love::NoGame                  },
    { 0,                 0                             }
};
// clang-format on

#include <modules/system_ext.hpp>

static void addCompatibilityAlias(lua_State* L, const char* module, const char* name,
                                  const char* alias)
{
    lua_getglobal(L, module);

    if (lua_istable(L, -1))
    {
        lua_getfield(L, -1, alias);
        bool hasAlias = !lua_isnoneornil(L, -1);
        lua_pop(L, 1);

        if (!hasAlias)
        {
            lua_getfield(L, -1, name);
            lua_setfield(L, -2, alias);
        }
    }

    lua_pop(L, 1);
}

int love::Initialize(lua_State* L)
{
    for (size_t i = 0; modules[i].name != nullptr; i++)
        luax::Preload(L, modules[i].func, modules[i].name);

    luax::InsistPinnedThread(L);
    luax::InsistGlobal(L, "love");

    // love._os
    lua_pushstring(L, System<Console::Which>::GetOS());
    lua_setfield(L, -2, "_os");

    // love._console
    lua_pushstring(L, __CONSOLE__);
    lua_setfield(L, -2, "_console");

    // love._potion_version
    lua_pushstring(L, __APP_VERSION__);
    lua_setfield(L, -2, "_potion_version");

    // love._version
    lua_pushstring(L, __LOVE_VERSION__);
    lua_setfield(L, -2, "_version");

    // love._version_(major, minor, micro, codename)
    lua_pushnumber(L, LOVE_FRAMEWORK.major);
    lua_setfield(L, -2, "_version_major");

    lua_pushnumber(L, LOVE_FRAMEWORK.minor);
    lua_setfield(L, -2, "_version_minor");

    lua_pushnumber(L, LOVE_FRAMEWORK.micro);
    lua_setfield(L, -2, "_version_revision");

    lua_pushstring(L, CODENAME);
    lua_setfield(L, -2, "_version_codename");

    lua_pushcfunction(L, love::OpenConsole);
    lua_setfield(L, -2, "_openConsole");

    lua_register(L, "print", love::Print);

    // love._potion_(major, minor, micro)
    lua_pushnumber(L, LOVE_POTION.major);
    lua_setfield(L, -2, "_potion_version_major");

    lua_pushnumber(L, LOVE_POTION.minor);
    lua_setfield(L, -2, "_potion_version_minor");

    lua_pushnumber(L, LOVE_POTION.micro);
    lua_setfield(L, -2, "_potion_version_revision");

    lua_newtable(L);
    for (int index = 0; love::COMPATIBILITY[index] != nullptr; index++)
    {
        lua_pushstring(L, love::COMPATIBILITY[index]);
        lua_rawseti(L, -2, index + 1);
    }
    lua_setfield(L, -2, "_version_compat");

    lua_pushcfunction(L, love::GetVersion);
    lua_setfield(L, -2, "getVersion");

    lua_pushcfunction(L, love::IsVersionCompatible);
    lua_setfield(L, -2, "isVersionCompatible");

    lua_pushstring(L, System<Console::Which>::GetOS());
    lua_setfield(L, -2, "_os");

    lua_pushstring(L, __CONSOLE__);
    lua_setfield(L, -2, "_console");

    luax::Require(L, "love.data");
    lua_pop(L, 1);

#if LUA_VERSION_NUM <= 501
    addCompatibilityAlias(L, "math", "fmod", "mod");
    addCompatibilityAlias(L, "string", "gmatch", "gfind");
#endif

    love::luasocket::preload(L);

    luax::Preload(L, luaopen_luautf8, "utf8");
    luax::Preload(L, luaopen_https, "https");

    return 1;
}

int love::OpenConsole(lua_State* L)
{
    struct sockaddr_in server;
    int lsockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (lsockfd < 0)
    {
        luax::PushBoolean(L, false);
        return 1;
    }

    /* make piepie happy :) */
    std::fill_n(&server, 1, sockaddr_in {});

    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = 0;
    server.sin_port        = htons(STDIO_PORT);

    if (bind(lsockfd, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        luax::PushBoolean(L, false);
        close(lsockfd);
        return 1;
    }

    if (listen(lsockfd, 5) < 0)
    {
        luax::PushBoolean(L, false);
        close(lsockfd);
        return 1;
    }

    int sockfd = accept(lsockfd, NULL, NULL);
    close(lsockfd);

    if (sockfd < 0)
    {
        luax::PushBoolean(L, false);
        return 1;
    }

    std::fflush(stdout);
    dup2(sockfd, STDOUT_FILENO);

    close(sockfd);

    luax::PushBoolean(L, true);
    return 1;
}

int love::Print(lua_State* L)
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

int love::GetVersion(lua_State* L)
{
    lua_pushinteger(L, LOVE_FRAMEWORK.major);
    lua_pushinteger(L, LOVE_FRAMEWORK.minor);
    lua_pushinteger(L, LOVE_FRAMEWORK.micro);
    lua_pushstring(L, CODENAME);

    return 4;
}

int love::SetGammaCorrect(lua_State* L)
{
    love::Graphics<>::SetGammaCorrect((bool)lua_toboolean(L, 1));

    return 0;
}

int love::IsVersionCompatible(lua_State* L)
{
    Version check {};

    if (lua_type(L, 1) == LUA_TSTRING)
        check = Version(luaL_checkstring(L, 1));
    else
    {
        int major    = luaL_checkinteger(L, 1);
        int minor    = luaL_checkinteger(L, 2);
        int revision = luaL_optinteger(L, 3, 0);

        check = Version(major, minor, revision);
    }

    for (auto& item : COMPATIBILITY)
    {
        if (check != item)
            continue;

        lua_pushboolean(L, true);
        return 1;
    }

    lua_pushboolean(L, false);

    return 1;
}

int love::LoadArgs(lua_State* L)
{
    if (luaL_loadbuffer(L, arg_lua, sizeof(arg_lua), "=[love \"arg.lua\"]") == 0)
        lua_call(L, 0, 1);

    return 1;
}

int love::LoadCallbacks(lua_State* L)
{
    if (luaL_loadbuffer(L, callbacks_lua, sizeof(callbacks_lua), "=[love \"callbacks.lua\"]") == 0)
        lua_call(L, 0, 1);

    return 1;
}

int love::Boot(lua_State* L)
{
    if (luaL_loadbuffer(L, boot_lua, sizeof(boot_lua), "=[love \"boot.lua\"]") == 0)
        lua_call(L, 0, 1);

    return 1;
}

int love::NoGame(lua_State* L)
{
    if (luaL_loadbuffer(L, nogame_lua, sizeof(nogame_lua), "=[love \"nogame.lua\"]") == 0)
        lua_call(L, 0, 1);

    return 1;
}
