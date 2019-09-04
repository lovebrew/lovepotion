/*
** modules/event.h
** @brief    : Handles swkbd
*/

#pragma once

class Keyboard
{
    public:
        static int Register(lua_State * L);
    
    private:
        //Löve2D Functions
        
        static int ShowTextInput(lua_State * L);

        //End Löve2D Functions
};
