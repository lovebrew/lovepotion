#pragma once

/*
** modules/mod_thread.h
** @brief    : Handles threading
*/

//#include "objects/thread/thread.h"
//#include "objects/thread/wrap_thread.h"

//#include "objects/channel/channel.h"
//#include "objects/channel/wrap_channel.h"

class LoveThread
{
    public:
        static void Exit();

        static int Register(lua_State * L);

    private:
        //std::vector<LuaThread *> threads;

        static int GetChannel(lua_State * L);
};
