#include "common/runtime.h"

#include "socket/http.h"
#include "socket/objects/socket.h"
#include "socket/objects/udp/wrap_udp.h"
#include "socket/objects/tcp/wrap_tcp.h"

/*
** Native LuaSocket Implementation
** See:
** UDP: http://w3.impa.br/~diego/software/luasocket/udp.html
** for more details on using LuaSocket
*/
int LuaSocket::InitHTTP(lua_State * L)
{
    luaL_Reg reg[] = 
    {
        { "request", httpRequest },
        { 0, 0 },
    };

    luaL_newlib(L, reg);

    return 1;
}

int LuaSocket::InitSocket(lua_State * L)
{
    int (*classes[])(lua_State *L) = 
    {
        initUDPClass,
        //initTCPClass,
        NULL,
    };

    for (int i = 0; classes[i]; i++) 
    {
        classes[i](L);
        lua_pop(L, 1);
    }

    luaL_Reg reg[] = 
    {
        //{ "bind",   tcpBind },
        //{ "tcp",    tcpNew  },
        { "udp",    udpNew  },
        { 0, 0 },
    };

    luaL_newlib(L, reg);

    return 1;
}

void LuaSocket::Close()
{
    socketExit();
}