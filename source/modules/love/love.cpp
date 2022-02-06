#include "common/luax.h"

#include "common/version.h"
#include "modules/love.h"

#include "modules/audio/wrap_audio.h"
#include "modules/data/wrap_datamodule.h"
#include "modules/event/wrap_event.h"
#include "modules/filesystem/wrap_filesystem.h"
#include "modules/font/wrap_fontmodule.h"
#include "modules/graphics/wrap_graphics.h"
#include "modules/image/wrap_imagemodule.h"
#include "modules/joystick/wrap_joystick.h"
#include "modules/keyboard/wrap_keyboard.h"
#include "modules/math/wrap_mathmodule.h"
#include "modules/physics/wrap_physics.h"
#include "modules/sound/wrap_sound.h"
#include "modules/system/wrap_system.h"
#include "modules/thread/wrap_threadmodule.h"
#include "modules/timer/wrap_timer.h"
#include "modules/touch/wrap_touch.h"
#include "modules/window/wrap_window.h"

#include "https/common/HTTPSCommon.h"
#include "luasocket/luasocket.h"

/* included scripts */
#include "nogame_lua.h"

// clang-format off
static constexpr char arg_lua[] =
#include "arg.lua"
;

static constexpr char callbacks_lua[] =
#include "callbacks.lua"
;

static constexpr char boot_lua[] =
#include "boot.lua"
;

static constexpr char console_lua[] =
#include "console.lua"
;

static constexpr luaL_Reg modules[] =
{
    { "love.audio",      Wrap_Audio::Register        },
    { "love.data",       Wrap_DataModule::Register   },
    { "love.event",      Wrap_Event::Register        },
    { "love.graphics",   Wrap_Graphics::Register     },
    { "love.filesystem", Wrap_Filesystem::Register   },
    { "love.font",       Wrap_FontModule::Register   },
    { "love.image",      Wrap_ImageModule::Register  },
    { "love.joystick",   Wrap_Joystick::Register     },
    { "love.keyboard",   Wrap_Keyboard::Register     },
    { "love.math",       Wrap_Math::Register         },
    { "love.physics",    Wrap_Physics::Register      },
    { "love.sound",      Wrap_Sound::Register        },
    { "love.system",     Wrap_System::Register       },
    { "love.thread",     Wrap_ThreadModule::Register },
    { "love.timer",      Wrap_Timer::Register        },
    { "love.touch",      Wrap_Touch::Register        },
    { "love.window",     Wrap_Window::Register       },
    { "love.nogame",     love::NoGame                },
    { "love.arg",        love::LoadArgs              },
    { "love.callbacks",  love::LoadCallbacks         },
    { "love.boot",       love::Boot                  },
    { "love.console",    love::OpenConsole           },
    { 0,                 0                           }
};
//clang-format on

int love::Initialize(lua_State* L)
{
    Luax::InsistPinnedThread(L);

    Luax::InsistGlobal(L, "love");

    //--------------CONSTANTS----------------//

    // love._os = "Horizon"
    lua_pushstring(L, "Horizon");
    lua_setfield(L, -2, "_os");

    // love._console_name
    lua_pushstring(L, __CONSOLE__);
    lua_setfield(L, -2, "_console_name");

    // love._potion_version
    lua_pushstring(L, __APP_VERSION__);
    lua_setfield(L, -2, "_potion_version");

    // love._version
    lua_pushstring(L, __LOVE_VERSION__);
    lua_setfield(L, -2, "_version");

    // love._version_(major, minor, revision, codename)
    lua_pushnumber(L, version::LOVE_FRAMEWORK.Major());
    lua_setfield(L, -2, "_version_major");

    lua_pushnumber(L, version::LOVE_FRAMEWORK.Minor());
    lua_setfield(L, -2, "_version_minor");

    lua_pushnumber(L, version::LOVE_FRAMEWORK.Revision());
    lua_setfield(L, -2, "_version_revision");

    lua_pushstring(L, version::CODENAME);
    lua_setfield(L, -2, "_version_codename");

    // love._potion_(major, minor, revision)
    lua_pushnumber(L, version::LOVE_POTION.Major());
    lua_setfield(L, -2, "_potion_version_major");

    lua_pushnumber(L, version::LOVE_POTION.Minor());
    lua_setfield(L, -2, "_potion_version_minor");

    lua_pushnumber(L, version::LOVE_POTION.Revision());
    lua_setfield(L, -2, "_potion_version_revision");

    // End Constants -- namespace functions //

    lua_pushcfunction(L, GetVersion);
    lua_setfield(L, -2, "getVersion");

    lua_pushcfunction(L, EnableAccelerometerAsJoystick);
    lua_setfield(L, -2, "_setAccelerometerAsJoystick");

    lua_pushcfunction(L, IsVersionCompatible);
    lua_setfield(L, -2, "isVersionCompatible");

    //---------------------------------------//

    // preload all the modules
    for (size_t i = 0; modules[i].name != nullptr; i++)
        Luax::Preload(L, modules[i].func, modules[i].name);

    Luax::Require(L, "love.data");
    lua_pop(L, 1);

    // LuaSocket
    love::luasocket::__open(L);

    // lua 5.3 stuff
    Luax::Preload(L, luaopen_luautf8, "utf8");

    // https module
    Luax::Preload(L, luaopen_https, "https");

    return 1;
}

int love::EnableAccelerometerAsJoystick(lua_State* L)
{
    // bool enable = lua_toboolean(L, 1);

    return 0;
}

int love::LoadArgs(lua_State* L)
{
    if (luaL_loadbuffer(L, arg_lua, sizeof(arg_lua), "=[love \"arg.lua\"]") == 0)
        lua_call(L, 0, 1);

    return 1;
}

int love::LoadCallbacks(lua_State*L)
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

int love::OpenConsole(lua_State* L)
{
    if (luaL_loadbuffer(L, console_lua, sizeof(console_lua), "=[love \"console.lua\"]") == 0)
        lua_call(L, 0, 1);

    return 1;
}

int love::NoGame(lua_State* L)
{
    if (!luaL_loadbuffer(L, (const char*)nogame_lua, nogame_lua_size, "=[love \"nogame.lua\"]"))
        lua_call(L, 0, 1);

    return 1;
}


int love::GetVersion(lua_State* L)
{
    lua_pushinteger(L, version::LOVE_FRAMEWORK.Major());
    lua_pushinteger(L, version::LOVE_FRAMEWORK.Minor());
    lua_pushinteger(L, version::LOVE_FRAMEWORK.Revision());
    lua_pushstring(L,  version::CODENAME);

    return 4;
}

int love::IsVersionCompatible(lua_State* L)
{
    std::string version;

    if (lua_type(L, 1) == LUA_TSTRING)
    {
        version = luaL_checkstring(L, 1);

        if (std::count(version.begin(), version.end(), '.') < 2)
            version.append(".0");
    }
    else
    {
        int major    = luaL_checkinteger(L, 1);
        int minor    = luaL_checkinteger(L, 2);
        int revision = luaL_optinteger(L, 3, 0);

        sprintf(version.data(), "%d.%d.%d", major, minor, revision);
    }

    for (size_t i = 0; version::COMPATABILITY[i]; i++)
    {
        std::string_view v(version::COMPATABILITY[i]);

        if (version.compare(v) != 0)
            continue;

        lua_pushboolean(L, true);
        return 1;
    }

    lua_pushboolean(L, false);

    return 1;
}
