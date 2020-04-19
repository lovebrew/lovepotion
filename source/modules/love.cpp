#include "common/runtime.h"

#include "common/version.h"
#include "modules/love.h"

#include "common/backend/input.h"

#include "modules/audio/wrap_audio.h"
#include "modules/data/wrap_datamodule.h"
#include "modules/event/wrap_event.h"
#include "modules/filesystem/wrap_filesystem.h"
#include "modules/graphics/wrap_graphics.h"
#include "modules/joystick/wrap_joystick.h"
#include "modules/math/wrap_mathmodule.h"
#include "modules/sound/wrap_sound.h"
#include "modules/system/wrap_system.h"
#include "modules/timer/wrap_timer.h"
#include "modules/touch/wrap_touch.h"
#include "modules/window/wrap_window.h"

#include "boot_lua.h"

/*
** @func Initialize
** Initializes the framework
*/
int Love::Initialize(lua_State * L)
{
    Luax::InsistPinnedThread(L);

    Luax::InsistGlobal(L, "love");

    //--------------CONSTANTS----------------//

    // love._os = "Horizon"
    lua_pushstring(L, "Horizon");
    lua_setfield(L, -2, "_os");

    //love._console_name
    lua_pushstring(L, LOVE_POTION_CONSOLE);
    lua_setfield(L, -2, "_console_name");

    //love._potion_version
    lua_pushstring(L, Version::LOVE_POTION);
    lua_setfield(L, -2, "_potion_version");

    // love._version
    lua_pushstring(L, Version::LOVE);
    lua_setfield(L, -2, "_version");

    // love._(major, minor, revision, codename)
    lua_pushnumber(L, Version::MAJOR);
    lua_setfield(L, -2, "_version_major");

    lua_pushnumber(L, Version::MINOR);
    lua_setfield(L, -2, "_version_minor");

    lua_pushnumber(L, Version::REVISION);
    lua_setfield(L, -2, "_version_revision");

    lua_pushstring(L, Version::CODENAME);
    lua_setfield(L, -2, "_version_codename");

    // End Constants -- namespace functions //

    lua_pushcfunction(L, GetVersion);
    lua_setfield(L, -2, "getVersion");

    lua_pushcfunction(L, EnsureApplicationType);
    lua_setfield(L, -2, "_ensureApplicationType");

    //---------------------------------------//

    Love::modules =
    {{
        { "love.audio",       Wrap_Audio::Register       },
        { "love.data",        Wrap_Data::Register,       },
        { "love.event",       Wrap_Event::Register,      },
        { "love.graphics",    Wrap_Graphics::Register,   },
        { "love.filesystem",  Wrap_Filesystem::Register, },
        { "love.joystick",    Wrap_Joystick::Register,   },
        { "love.math",        Wrap_Math::Register        },
        { "love.sound",       Wrap_Sound::Register       },
        { "love.system",      Wrap_System::Register      },
        { "love.timer",       Wrap_Timer::Register,      },
        { "love.touch",       Wrap_Touch::Register       },
        { "love.window",      Wrap_Window::Register,     },
        { "love.boot",        Boot                       },
        { 0 }
    }};

    // preload all the modules
    for (int i = 0; Love::modules[i].name  != nullptr; i++)
        Love::Preload(L, Love::modules[i].reg, Love::modules[i].name);

    Luax::Require(L, "love.data");
    lua_pop(L, 1);

    Love::Preload(L, luaopen_luautf8, "utf8");
    // love_preload(L, LuaSocket::InitSocket, "socket");
    // love_preload(L, LuaSocket::InitHTTP,   "socket.http");

    return 1;
}

/*
** @func Boot
** Boots LOVE
*/
int Love::Boot(lua_State * L)
{
    if (Luax::DoBuffer(L, (const char *)boot_lua, boot_lua_size, "boot.lua"))
        LOG("boot.lua error: %s", lua_tostring(L, -1));

    return 1;
}

/*
** @func GetVersion
** Return the version for the framework.
*/
int Love::GetVersion(lua_State * L)
{
    lua_pushnumber(L, Version::MAJOR);
    lua_pushnumber(L, Version::MINOR);
    lua_pushnumber(L, Version::REVISION);
    lua_pushstring(L, Version::CODENAME);

    return 4;
}

//------------------------------//

/*
** @func CheckForTitleTakeover
** Checks for Title Takeover on atmosphère
** Does absolutely nothing on 3DS. Ran at boot once.
*/
int Love::EnsureApplicationType(lua_State * L)
{
    #if defined(__SWITCH__)
        AppletType type = appletGetAppletType();

        bool isApplicationType = (type != AppletType_Application ||
                                  type != AppletType_SystemApplication);

        if (isApplicationType)
            return 0;

        return luaL_error(L, TITLE_TAKEOVER_ERROR);
    #endif

    return 0;
}

/*
** @func GetField
** Gets a field from the love module.
** May return nil if it does not exist on the Lua stack.
*/
void Love::GetField(lua_State * L, const char * field)
{
    lua_getfield(L, LUA_GLOBALSINDEX, "love");
    lua_getfield(L, -1, field);
    lua_remove(L, -2);
}

/*
** @func Preload
** Preloads a Lua C function <func> into package.preload
** with <name>. See the Lua 5.1 manual for more details:
** https://www.lua.org/manual/5.1/manual.html#pdf-package.preload
*/
int Love::Preload(lua_State * L, lua_CFunction func, const char * name)
{
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    lua_pushcfunction(L, func);
    lua_setfield(L, -2, name);
    lua_pop(L, 2);

    return 0;
}

/*
** @func InsistRegistry
** Creates the @registry if necessary, pushing it to the stack
*/
int Love::InsistRegistry(lua_State * L, Registry registry)
{
    switch (registry)
    {
        case Registry::MODULES:
            return Luax::InsistLove(L, "_modules");
        case Registry::UNKNOWN:
        default:
            return luaL_error(L, "Attempted to use invalid registry");
    }
}

/*
** @func GetRegistry
** Gets the field for the Lua registry index <registry>.
** This is necessary to store userdata and push it later.
*/
int Love::GetRegistry(lua_State * L, Registry registry)
{
    switch (registry)
    {
        case Registry::OBJECTS:
            lua_getfield(L, LUA_REGISTRYINDEX, "_loveobjects");
            return 1;
        case Registry::MODULES:
            Luax::GetLove(L, "_modules");
        case Registry::UNKNOWN:
        default:
            return luaL_error(L, "Attempted to use invalid registry");
    }
}
