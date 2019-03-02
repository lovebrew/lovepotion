#include "common/runtime.h"

#include "common/version.h"
#include "common/types.h"
#include "common/util.h"

#include "objects/gamepad/gamepad.h"

#include "objects/object/object.h"
#include "objects/file/wrap_file.h"
#include "objects/font/wrap_font.h"
#include "objects/gamepad/wrap_gamepad.h"
#include "objects/image/wrap_image.h"
#include "objects/source/wrap_source.h"
#include "objects/quad/wrap_quad.h"

#include "modules/audio.h"
#include "modules/event.h"
#include "modules/filesystem.h"
#include "modules/graphics.h"
#include "modules/joystick.h"
#include "modules/keyboard.h"
// #include "modules/mod_math.h"
#include "modules/timer.h"
// #include "modules/system.h"
#include "modules/window_common.h"

#include "nogame_lua.h"

#if defined (_3DS)
struct { const char * name; void (*init)(void); int (*reg)(lua_State *L); void (*close)(void); } modules[] = 
{
    { "audio",      Audio::Initialize,      Audio::Register,      Audio::Exit      },
    { "event",      NULL,                   LoveEvent::Register,  NULL             },
    { "filesystem", NULL,                   Filesystem::Register, Filesystem::Exit },
    { "graphics",   Graphics::Initialize,   Graphics::Register,   NULL             },
    { "keyboard",   Keyboard::Initialize,   Keyboard::Register,   NULL             },
    { "joystick",   NULL,                   Joystick::Register,   NULL             },
    //{ "math",       NULL,                   Math::Register,       NULL             },
    //{ "system",     System::Initialize,     System::Register,     System::Exit     },
    //{ "thread",   NULL,                   LoveThread::Register, LoveThread::Exit },
    { "timer",      NULL,                   Timer::Register,      NULL             },
    //{ "touch",    NULL,                   Touch::Register,      NULL             },
    { "window",     NULL,                   Window::Register,     NULL             },
    { 0,            NULL,                   NULL,                 NULL             }
};
#endif

lua_State * loveState;
bool LOVE_QUIT = false;

void Love::InitModules(lua_State * L)
{
    Joystick::Initialize(L);

    for (int i = 0; modules[i].name; i++)
    {
        if (modules[i].init)
            modules[i].init();
    }
}

void Love::InitConstants(lua_State * L)
{
    lua_getglobal(L, "love");

    // love._constants
    // love._os = {"Horizon","Switch"}
    lua_newtable(L);
    lua_pushnumber(L, 1);
    lua_pushstring(L, "Horizon");
    lua_rawset(L, -3);
    lua_pushnumber(L, 2);
    lua_pushstring(L, "3DS");
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

int Love::Initialize(lua_State * L)
{
    int (*classes[])(lua_State *L) = 
    {
        /*initCanvasClass,
        initChannelClass,*/
        initFileClass,
        initFontClass,
        initGamepadClass,
        initImageClass,
        initQuadClass,
        initSourceClass,
        //initThreadClass,
        NULL
    };

    for (int i = 0; classes[i]; i++) 
    {
        classes[i](L);
        lua_pop(L, 1);
    }

    luaL_Reg reg[] =
    {
        { "getVersion",    GetVersion       },
        { "run",           Run              },
        { "_nogame",       NoGame           },
        //{ "scan",          Event::PollEvent },
        { 0, 0 }
    };

    luaL_newlib(L, reg);

    for (int i = 0; modules[i].name; i++)
    {
        modules[i].reg(L);
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
    
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    Graphics::Clear(GFX_TOP, GFX_LEFT);
    
    if (luaL_dostring(L, LOVE_DRAW))
        luaL_error(L, "%s", lua_tostring(L, -1));

    if (gfxIs3D())
    {
        Graphics::Clear(GFX_TOP, GFX_RIGHT);

        if (luaL_dostring(L, LOVE_DRAW))
            luaL_error(L, "%s", lua_tostring(L, -1));
    }

    Graphics::Clear(GFX_BOTTOM, GFX_LEFT);
    
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

int Love::NoGame(lua_State * L)
{
    luaL_dobuffer(L, (char *)nogame_lua, nogame_lua_size, "nogame");

    return 0;
}

void Love::Exit(lua_State * L)
{
    for (int i = 0; modules[i].name; i++)
    {
        if (modules[i].close)
            modules[i].close();
    }

    lua_close(L);

    //Window::Exit();
    Graphics::Exit();
}
