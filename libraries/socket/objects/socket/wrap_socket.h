#pragma once

#include "objects/socket/socket.h"

namespace Wrap_Socket
{
    luaL_Reg functions[5];

    int SetTimeout(lua_State * L);

    int GetPeerName(lua_State * L);

    int GetSockName(lua_State * L);

    int Close(lua_State * L);

    int Register(lua_State * L);
}
