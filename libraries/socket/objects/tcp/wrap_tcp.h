#pragma once

#include "socket/objects/tcp/tcp.h"
#include "objects/socket/wrap_socket.h"

namespace Wrap_TCP
{
    int New(lua_State * L);

    int New4(lua_State * L);

    int Accept(lua_State * L);

    int Bind(lua_State * L);

    int Connect(lua_State * L);

    int GetStats(lua_State * L);

    int GetPeerName(lua_State * L);

    int GetSockName(lua_State * L);

    int Listen(lua_State * L);

    int Receive(lua_State * L);

    int Send(lua_State * L);

    int SetOption(lua_State * L);

    int SetStats(lua_State * L);

    int Shutdown(lua_State * L);

    int ToString(lua_State * L);

    TCP * CheckTCPSocket(lua_State * L, int index);

    bool CheckTCPSocketType(lua_State * L, const std::string & type, int index);

    int Register(lua_State * L);
}
