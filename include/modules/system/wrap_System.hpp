#pragma once

#include "common/luax.hpp"
#include "modules/system/System.hpp"

namespace Wrap_System
{
    int getProcessorCount(lua_State* L);

    int getPowerInfo(lua_State* L);

    int getClipboardText(lua_State* L);

    int setClipboardText(lua_State* L);

    int vibrate(lua_State* L);

    int openURL(lua_State* L);

    int hasBackgroundMusic(lua_State* L);

    int getPreferredLocales(lua_State* L);

    int getNetworkInfo(lua_State* L);

    int getProductInfo(lua_State* L);

    int getOS(lua_State* L);

    int getFriendInfo(lua_State* L);

    int getPlayCoins(lua_State* L);

    int setPlayCoins(lua_State* L);

    int getTheme(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_System
