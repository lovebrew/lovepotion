#include "common/runtime.h"
#include "modules/mod_thread.h"

#include "objects/thread/thread.h"
#include "objects/thread/wrap_thread.h"

#include "objects/channel/channel.h"
#include "objects/channel/wrap_channel.h"

int LoveThread::GetChannel(lua_State * L)
{
    const char * name = luaL_checkstring(L, 1);

    if (channels.find(name) == channels.end()) //create one
    {
        int ret = channelNew(L);

        return ret;
    }
    else
    {
        Channel * self = channels[name];
        love_push_userdata(L, self);
    }

    return 0;
}

void LoveThread::Exit()
{
    //for (auto thread : threads)
    //    thread->Collect();
}

int LoveThread::Register(lua_State * L)
{
    luaL_Reg reg[] = 
    {
        { "getChannel", GetChannel },
        { "newChannel", channelNew },
        { "newThread",  threadNew  },
        { 0, 0 }
    };
    
    luaL_newlib(L, reg);

    return 1;
}