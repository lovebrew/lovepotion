#pragma once

/*
** modules/mod_thread.h
** @brief    : Handles threading
*/

namespace LoveThread
{
    int GetChannel(lua_State * L);

    void Exit();

    int Register(lua_State * L);
}

//extern std::vector<ThreadClass *> threads;