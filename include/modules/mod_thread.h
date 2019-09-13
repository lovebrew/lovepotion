#pragma once

/*
** modules/mod_thread.h
** @brief    : Handles threading
*/

class LoveThread
{
    public:
        static void Exit();

        static int Register(lua_State * L);

    private:
        static int GetChannel(lua_State * L);
};
