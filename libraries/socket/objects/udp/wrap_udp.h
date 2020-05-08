#pragma once

#include "objects/udp/udp.h"
#include "objects/socket/wrap_socket.h"

namespace Wrap_UDP
{
    int New(lua_State * L);

    int GetPeerName(lua_State * L);

    int SetPeerName(lua_State * L);

    int SetSockName(lua_State * L);

    int Send(lua_State * L);

    int SendTo(lua_State * L);

    int Receive(lua_State * L);

    int ReceiveFrom(lua_State * L);

    int SetOption(lua_State * L);

    int ToString(lua_State * L);

    UDP * CheckUDPSocket(lua_State * L, int index);

    bool CheckUDPSocketType(lua_State * L, const std::string & type, int index);

    int Register(lua_State * L);
}
