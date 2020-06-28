#include "modules/thread/threadmodule.h"

#include "objects/thread/luathread.h"
#include "objects/thread/wrap_luathread.h"

#include "objects/channel/channel.h"
#include "objects/channel/wrap_channel.h"

namespace Wrap_ThreadModule
{
    int NewThread(lua_State * L);

    int NewChannel(lua_State * L);

    int GetChannel(lua_State * L);

    int Register(lua_State * L);
}
