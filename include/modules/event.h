/*
** modules/event.h
** @brief    : Handles events.. like quitting.. and other things
*/

#pragma once

namespace LoveEvent
{
    int PollEvent(lua_State * L);

    //Löve2D Functions

    int Quit(lua_State * L);

    int Poll(lua_State * L);

    //End Löve2D Functions

    int Register(lua_State * L);
}