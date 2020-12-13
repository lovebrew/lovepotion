#pragma once

#include "modules/window/window.h"

namespace Wrap_Window
{
    int Close(lua_State * L);

    int GetDisplayCount(lua_State * L);

    int GetFullscreenModes(lua_State * L);

    int IsOpen(lua_State * L);

    int SetMode(lua_State * L);

    int Register(lua_State * L);
}
