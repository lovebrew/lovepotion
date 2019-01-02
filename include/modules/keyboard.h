/*
** modules/event.h
** @brief    : Handles swkbd
*/

#pragma once

namespace Keyboard
{
    //Löve2D Functions

    int ShowTextInput(lua_State * L);

    //End Löve2D Functions

    int Register(lua_State * L);
}