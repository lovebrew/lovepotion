/*
** modules/event.h
** @brief    : Handles swkbd
*/

#pragma once

namespace Keyboard
{
    void Initialize();

    //Löve2D Functions

    int SetTextInput(lua_State * L);

    //End Löve2D Functions

    int Register(lua_State * L);
}