/*
** modules/keyboard.h
** @brief    : Handles swkbd
*/

#pragma once

namespace Keyboard
{
    void Initialize();

    int ShowTextInput(lua_State * L);

    int Register(lua_State * L);
};