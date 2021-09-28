#pragma once

#include "common/luax.h"
#include "modules/window/window.h"
namespace Wrap_Window
{
    int Close(lua_State* L);

    int GetDisplayCount(lua_State* L);

    int GetFullscreenModes(lua_State* L);

    int IsOpen(lua_State* L);

    int SetMode(lua_State* L);

    int LoadButtons(lua_State* L);

    int LoadMessageBox(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Window
