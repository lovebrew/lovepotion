#include "common/runtime.h"
#include <unistd.h>
#include <stdarg.h>

#include "common/version.h"

#include "objects/gamepad/gamepad.h"
#include "objects/gamepad/wrap_gamepad.h"

#include "modules/audio.h"
#include "modules/event.h"
#include "modules/filesystem.h"
#include "modules/graphics.h"
#include "modules/joystick.h"
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

struct { const char * name; int (*fn)(lua_State *L); void (*close)(void); } modules[] = 
{
    { "audio",        Audio::Register,        Audio::Exit            },
    { "event",        Event::Register,        NULL                },
    { "filesystem",    Filesystem::Register,    Filesystem::Exit    },
    { "graphics",    Graphics::Register,        NULL                },
    { "joystick",    Joystick::Register,        NULL                },
    { "math",        Math::Register,            NULL                },
    { "system",        System::Register,        System::Exit        },
    { "timer",        Timer::Register,        NULL                },
    { "touch",        Touch::Register,        NULL                },
    { "thread",        LoveThread::Register,    LoveThread::Exit    },
    { "window",        Window::Register,        NULL                },
    { 0 }
};

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
        { "scan",            Event::PollEvent},
        { "_nogame",        NoGame            },
        { "run",            Run                },
        { "getVersion",        GetVersion        },
        { "enableConsole",    EnableConsole    },
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
    Event::PollEvent(L);

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
    Console::Initialize();

    return 0;
}

int Love::NoGame(lua_State * L)
{
    chdir("romfs:/");

    if (luaL_dofile(L, "main.lua"))
        return luaL_error(L, "%s", lua_tostring(L, -1));

    return 0;
}

void Love::Exit(lua_State * L)
{
    controllers.clear();

    for (int i = 0; modules[i].close; i++)
        modules[i].close();

    lua_close(L);

    Window::Exit();
    Graphics::Exit();
}
