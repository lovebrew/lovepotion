#include "common/runtime.h"

#include "objects/gamepad/gamepad.h"

#include "objects/object/object.h"
#include "objects/file/wrap_file.h"
#include "objects/font/wrap_font.h"
#include "objects/gamepad/wrap_gamepad.h"
#include "objects/image/wrap_image.h"
#include "objects/source/wrap_source.h"
#include "objects/quad/wrap_quad.h"
#include "objects/randomgenerator/wrap_randomgenerator.h"

#include "modules/audio.h"
#include "modules/event.h"
#include "modules/filesystem.h"
#include "modules/graphics.h"
#include "modules/joystick.h"
#include "modules/keyboard.h"
#include "modules/mod_math.h"
#include "modules/mod_thread.h"
#include "modules/timer.h"
#include "modules/touch.h"
#include "modules/system.h"
#include "modules/window.h"
#include "modules/display.h"

love_modules modules[13] =
{
    { "audio",      Audio::Register,       Audio::Exit      },
    { "event",      LoveEvent::Register,   NULL             },
    { "filesystem", Filesystem::Register,  Filesystem::Exit },
    { "graphics",   Graphics::Register,    NULL             },
    { "joystick",   Joystick::Register,    NULL             },
    { "keyboard",   Keyboard::Register,    NULL             },
    { "math",       Math::Register,        NULL             },
    { "system",     System::Register,      System::Exit     },
    { "thread",     LoveThread::Register,  LoveThread::Exit },
    { "timer",      Timer::Register,       NULL             },
    { "touch",      Touch::Register,       NULL             },
    { "window",     Window::Register,      NULL             },
    { 0 }
};

int (*classes[])(lua_State *L) =
{
    initFileClass,
    initFontClass,
    initGamepadClass,
    initImageClass,
    initQuadClass,
    initSourceClass,
    initRandomGeneratorClass,
    NULL
};

//love.run
int Love::Run(lua_State * L)
{
    LoveEvent::Pump(L);

    luaL_dostring(L, LOVE_TIMER_STEP);

    if (luaL_dostring(L, LOVE_UPDATE))
        luaL_error(L, "%s", lua_tostring(L, -1));

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    Display::Clear(0);

    if (luaL_dostring(L, LOVE_DRAW))
        luaL_error(L, "%s", lua_tostring(L, -1));

    if (gfxIs3D())
    {
        Display::Clear(2);

        if (luaL_dostring(L, LOVE_DRAW))
            luaL_error(L, "%s", lua_tostring(L, -1));
    }

    Display::Clear(1);

    if (luaL_dostring(L, LOVE_DRAW))
        luaL_error(L, "%s", lua_tostring(L, -1));

    C3D_FrameEnd(0);

    Timer::Tick();

    return 0;
}
