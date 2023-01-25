#include <common/luax.hpp>
#include <common/version.hpp>

#include <common/HTTPSCommon.h>
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

static constexpr char logfile_lua[] = {
#include "scripts/logfile.lua"
};

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
#include <modules/sensor/wrap_sensor.hpp>
#include <modules/sound/wrap_sound.hpp>
#include <modules/system/wrap_system.hpp>
#include <modules/thread/wrap_threadmodule.hpp>
#include <modules/timer/wrap_timer.hpp>
#include <modules/touch/wrap_touch.hpp>
#include <modules/window/wrap_window.hpp>

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
    { "love.sensor",     Wrap_Sensor::Register         },
    { "love.sound",      Wrap_Sound::Register          },
    { "love.system",     Wrap_System::Register         },
    { "love.thread",     Wrap_ThreadModule::Register   },
    { "love.timer",      Wrap_Timer::Register          },
    { "love.touch",      Wrap_Touch::Register          },
    { "love.window",     Wrap_Window::Register         },
    #if defined(__DEBUG__)
    { "love.log",        love::LoadLogFile             },
    #endif
    { "love.arg",        love::LoadArgs                },
    { "love.callbacks",  love::LoadCallbacks           },
    { "love.boot",       love::Boot                    },
    { "love.nogame",     love::NoGame                  },
    { 0,                 0                             }
};
// clang-format on

#include <modules/system_ext.hpp>

int love::Initialize(lua_State* L)
{
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

    // love._potion_(major, minor, micro)
    lua_pushnumber(L, LOVE_POTION.major);
    lua_setfield(L, -2, "_potion_version_major");

    lua_pushnumber(L, LOVE_POTION.minor);
    lua_setfield(L, -2, "_potion_version_minor");

    lua_pushnumber(L, LOVE_POTION.micro);
    lua_setfield(L, -2, "_potion_version_revision");

    lua_pushcfunction(L, GetVersion);
    lua_setfield(L, -2, "getVersion");

    lua_pushcfunction(L, IsVersionCompatible);
    lua_setfield(L, -2, "isVersionCompatible");

    for (size_t i = 0; modules[i].name != nullptr; i++)
        luax::Preload(L, modules[i].func, modules[i].name);

    luax::Require(L, "love.data");
    lua_pop(L, 1);

    love::luasocket::preload(L);

    luax::Preload(L, luaopen_luautf8, "utf8");
    luax::Preload(L, luaopen_https, "https");

    return 1;
}

int love::GetVersion(lua_State* L)
{
    lua_pushinteger(L, LOVE_FRAMEWORK.major);
    lua_pushinteger(L, LOVE_FRAMEWORK.minor);
    lua_pushinteger(L, LOVE_FRAMEWORK.micro);
    lua_pushstring(L, CODENAME);

    return 4;
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

int love::LoadLogFile(lua_State* L)
{
    if (luaL_loadbuffer(L, logfile_lua, sizeof(logfile_lua), "=[love \"logfile.lua\"]") == 0)
        lua_call(L, 0, 1);

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
