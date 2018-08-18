#include "common/runtime.h"

#include "socket/objects/socket.h"
#include "socket/objects/udp/wrap_udp.h"
#include "socket/objects/tcp/wrap_tcp.h"

/*
** Native LuaSocket Implementation
** See:
** UDP: http://w3.impa.br/~diego/software/luasocket/udp.html
** for more details on using LuaSocket
*/

int LuaSocket::Initialize(lua_State * L)
{
    int (*classes[])(lua_State *L) = 
    {
        initUDPClass,
        initTCPClass,
        NULL,
    };

    for (int i = 0; classes[i]; i++) 
    {
        classes[i](L);
        lua_pop(L, 1);
    }

    luaL_Reg reg[] = 
    {
        { "udp",    udpNew  },
        { "tcp",    tcpNew  },
        { "bind",   tcpBind },
        { 0, 0 },
    };

    luaL_newlib(L, reg);

    return 1;
}

void LuaSocket::Close()
{
    socketExit();
}