#pragma once

#include "objects/socket/socket.h"

namespace Wrap_Socket
{
    int New(lua_State * L);

    int SetTimeout(lua_State * L);

    int GarbageCollect(lua_State * L);

    int GetPeerName(lua_State * L);

    int GetSockName(lua_State * L);

    int Close(lua_State * L);

    int ToString(lua_State * L);

    Socket * CheckSocket(lua_State * L, int index);

    int Register(lua_State * L);
}
