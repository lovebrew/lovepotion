#include "common/runtime.h"

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
#include "modules/mod_math.h"
#include "modules/timer.h"
#include "modules/system.h"
#include "modules/window_common.h"

love_modules modules[12] =
{
    { "audio",      Audio::Initialize,      Audio::Register,      Audio::Exit      },
    { "event",      NULL,                   LoveEvent::Register,  NULL             },
    { "filesystem", NULL,                   Filesystem::Register, Filesystem::Exit },
    { "graphics",   Graphics::Initialize,   Graphics::Register,   NULL             },
    { "keyboard",   Keyboard::Initialize,   Keyboard::Register,   NULL             },
    { "joystick",   NULL,                   Joystick::Register,   NULL             },
    { "math",       NULL,                   Math::Register,       NULL             },
    { "system",     System::Initialize,     System::Register,     System::Exit     },
    { "timer",      NULL,                   Timer::Register,      NULL             },
    //{ "touch",    NULL,                   Touch::Register,      NULL             },
    { "window",     NULL,                   Window::Register,     NULL             },
    { 0,            NULL,                   NULL,                 NULL             }
};

int (*classes[])(lua_State *L) = 
{
    initFileClass,
    initFontClass,
    initGamepadClass,
    initImageClass,
    initQuadClass,
    initSourceClass,
    NULL
};

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