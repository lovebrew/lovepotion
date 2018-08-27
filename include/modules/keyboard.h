/*
** modules/keyboard.h
** @brief    : Handles swkbd
*/

#pragma once

namespace Keyboard
{
    void Initialize();

    void Update(lua_State * L);

    int SetTextInput(lua_State * L);

    int Register(lua_State * L);
};