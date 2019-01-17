#include "common/runtime.h"
#include <unistd.h>
#include <stdarg.h>

#include "common/version.h"

#include "objects/gamepad/gamepad.h"
#include "objects/gamepad/wrap_gamepad.h"

#include "objects/channel/channel.h"

#include "modules/audio.h"
#include "modules/event.h"
#include "modules/filesystem.h"
#include "modules/graphics.h"
#include "modules/joystick.h"
#include "modules/keyboard.h"
#include "modules/mod_math.h"
#include "modules/mod_thread.h"
#include "modules/system.h"
#include "modules/timer.h"
#include "modules/touch.h"
#include "modules/window.h"

#include "objects/canvas/wrap_canvas.h"
#include "objects/channel/wrap_channel.h"
#include "objects/file/wrap_file.h"
#include "objects/font/wrap_font.h"
#include "objects/image/wrap_image.h"
#include "objects/quad/wrap_quad.h"
#include "objects/source/wrap_source.h"
#include "objects/thread/wrap_thread.h"

#include "nogame_lua.h"

struct { const char * name; void (*open)(void); int (*fn)(lua_State *L); void (*close)(void); } modules[] = 
{
    { "audio",      Audio::Initialize,      Audio::Register,      Audio::Exit      },
    { "event",      NULL,                   LoveEvent::Register,  NULL             },
    { "filesystem", Filesystem::Initialize, Filesystem::Register, Filesystem::Exit },
    { "graphics",   Graphics::Initialize,   Graphics::Register,   NULL             },
    { "joystick",   NULL,                   Joystick::Register,   NULL             },
    { "keyboard",   NULL,                   Keyboard::Register,   NULL             },
    { "math",       NULL,                   Math::Register,       NULL             },
    { "system",     System::Initialize,     System::Register,     System::Exit     },
    { "thread",     NULL,                   LoveThread::Register, LoveThread::Exit },
    { "timer",      NULL,                   Timer::Register,      NULL             },
    { "touch",      NULL,                   Touch::Register,      NULL             },
    { "window",     Window::Initialize,     Window::Register,     NULL             },
    { 0 }
};

void Love::InitModules(lua_State * L)
{
    for (int i = 0; modules[i].name; i++)
    {
        if (modules[i].open)
            modules[i].open();
    }

    Joystick::Initialize(L);
    
    socketInitializeDefault();
}

void Love::InitConstants(lua_State * L)
{
    lua_getglobal(L, "love");

    // love._constants
    // love._os = {"Horizon","Switch"}
    lua_newtable(L);
    lua_pushnumber(L, 1);
    lua_pushstring(L,"Horizon");
    lua_rawset(L, -3);
    lua_pushnumber(L, 2);
    lua_pushstring(L,"Switch");
    lua_rawset(L, -3);
    lua_setfield(L, -2, "_os");

    // love._version stuff
    lua_pushstring(L, Love::VERSION.c_str());
    lua_setfield(L, -2, "_version");
    lua_pushnumber(L, Love::VERSION_MAJOR);
    lua_setfield(L, -2, "_version_major");
    lua_pushnumber(L, Love::VERSION_MINOR);
    lua_setfield(L, -2, "_version_minor");
    lua_pushnumber(L, Love::VERSION_REVISION);
    lua_setfield(L, -2, "_version_revision");
    lua_pushstring(L, Love::CODENAME.c_str());
    lua_setfield(L, -2, "_version_codename");

    lua_pop(L, 1);
}

lua_State * loveState;
int Love::Initialize(lua_State * L)
{
    int (*classes[])(lua_State *L) = 
    {
        initCanvasClass,
        initChannelClass,
        initFileClass,
        initFontClass,
        initGamepadClass,
        initImageClass,
        initQuadClass,
        initSourceClass,
        initThreadClass,
        NULL
    };

    for (int i = 0; classes[i]; i++) 
    {
        classes[i](L);
        lua_pop(L, 1);
    }

    luaL_Reg reg[] =
    {
        { "_nogame",       NoGame               },
        { "enableConsole", EnableConsole        },
        { "getVersion",    GetVersion           },
        { "run",           Run                  },
        { "scan",          LoveEvent::PollEvent },
        { 0, 0 }
    };

    luaL_newlib(L, reg);

    for (int i = 0; modules[i].name; i++)
    {
        modules[i].fn(L);
        lua_setfield(L, -2, modules[i].name);
    }

    love_get_registry(L, OBJECTS);
    if (!lua_istable(L, -1))
    {
        lua_newtable(L);
        lua_replace(L, -2);

        lua_setfield(L, LUA_REGISTRYINDEX, "_loveobjects");
    }

    loveState = L;

    return 1;
}

bool Love::IsRunning()
{
    return LOVE_QUIT == false;
}

int Love::RaiseError(const char * format, ...)
{
    char buffer[256];

    va_list args;

    va_start(args, format); 

    vsprintf(buffer, format, args);

    va_end(args);

    return luaL_error(loveState, buffer, "");
}

//love.run
int Love::Run(lua_State * L)
{
    LoveEvent::PollEvent(L);

    luaL_dostring(L, LOVE_TIMER_STEP);

    if (luaL_dostring(L, LOVE_UPDATE))
        luaL_error(L, "%s", lua_tostring(L, -1));

    Graphics::Clear(L);
    
    if (luaL_dostring(L, LOVE_DRAW))
        luaL_error(L, "%s", lua_tostring(L, -1));

    Graphics::Present(L);

    Timer::Tick();

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

    lua_pushnumber(L, Love::VERSION_MAJOR);
    lua_pushnumber(L, Love::VERSION_MINOR);
    lua_pushnumber(L, Love::VERSION_REVISION);
    lua_pushstring(L, Love::CODENAME.c_str());

    return 4;
}

int Love::EnableConsole(lua_State * L)
{
    return 0;
}

int Love::NoGame(lua_State * L)
{
    luaL_dobuffer(L, (char *)nogame_lua, nogame_lua_size, "nogame");

    return 0;
}

void Love::Exit(lua_State * L)
{
    controllers.clear();

    for (int i = 0; modules[i].name; i++)
    {
        if (modules[i].close)
            modules[i].close();
    }

    lua_close(L);

    Window::Exit();
    Graphics::Exit();
}
