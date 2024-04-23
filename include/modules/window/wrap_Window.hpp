#pragma once

#include "common/luax.hpp"
#include "modules/window/Window.hpp"

namespace Wrap_Window
{
    int getDisplayCount(lua_State* L);

    int getDisplayName(lua_State* L);

    int setMode(lua_State* L);

    int updateMode(lua_State* L);

    int getMode(lua_State* L);

    int isHighDPIAllowed(lua_State* L);

    int getDisplayOrientation(lua_State* L);

    int getFullscreenModes(lua_State* L);

    int getFullscreen(lua_State* L);

    int isOpen(lua_State* L);

    int close(lua_State* L);

    int getDesktopDimensions(lua_State* L);

    int setVSync(lua_State* L);

    int getVSync(lua_State* L);

    int setDisplaySleepEnabled(lua_State* L);

    int isDisplaySleepEnabled(lua_State* L);

    int hasFocus(lua_State* L);

    int isVisible(lua_State* L);

    int getDPIScale(lua_State* L);

    int getNativeDPIScale(lua_State* L);

    int toPixels(lua_State* L);

    int fromPixels(lua_State* L);

    int isMaximized(lua_State* L);

    int isMinimized(lua_State* L);

    int showMessageBox(lua_State* L);

    int setIcon(lua_State* L);

    int setTitle(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_Window
