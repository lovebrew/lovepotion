#pragma once

#include "common/luax.hpp"
#include "modules/keyboard/Keyboard.hpp"

namespace Wrap_Keyboard
{
    int setKeyRepeat(lua_State* L);

    int hasKeyRepeat(lua_State* L);

    int setTextInput(lua_State* L);

    int hasTextInput(lua_State* L);

    int hasScreenKeyboard(lua_State* L);

    int isDown(lua_State* L);

    int isScancodeDown(lua_State* L);

    int getScancodeFromKey(lua_State* L);

    int getKeyFromScancode(lua_State* L);

    int isModifierActive(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_Keyboard
