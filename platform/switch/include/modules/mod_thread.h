#pragma once

/*
** modules/mod_thread.h
** @brief    : Handles threading
*/

class Thread
{
    public:
        static void Exit();

        static int Register(lua_State * L);

    private:
        std::vector<LuaThread *> threads;

        int GetChannel(lua_State * L);
};
