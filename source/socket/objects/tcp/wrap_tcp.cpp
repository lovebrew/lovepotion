#include "common/runtime.h"
#include "socket/common.h"

#include "socket/objects/socket.h"

#include "socket/objects/tcp/tcp.h"
#include "socket/objects/tcp/wrap_tcp.h"

#define CLASS_NAME "TCP"
#define CLASS_TYPE LUAOBJ_TYPE_TCP

int tcpNew(lua_State * L)
{
    void * raw_self = luaobj_newudata(L, sizeof(TCP));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    TCP * self = new (raw_self) TCP();

    return 1;
}

int tcpNew(lua_State * L, int sockfd)
{
    void * raw_self = luaobj_newudata(L, sizeof(TCP));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    TCP * self = new (raw_self) TCP(sockfd);

    return 1;
}

int tcpAccept(lua_State * L)
{
    TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    int newSocket = self->Accept();

    if (newSocket < 0)
        return 0;

    int succ = tcpNew(L, newSocket);

    return succ;
}

int tcpGetSockName(lua_State * L)
{
    char ip[0x40];

    TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);
    self->GetSockName(ip);

    lua_pushstring(L, ip);
    lua_pushnumber(L, self->GetPort());

    return 2;
}

int tcpBind(lua_State * L)
{
    int succ;
    
    string ip;
    int port;

    if (lua_type(L, 1) != LUA_TUSERDATA)
	{
        ip = luaL_checkstring(L, 1);
	    port = luaL_checkinteger(L, 2);

        int ret = tcpNew(L);

        TCP * self = (TCP *)luaobj_checkudata(L, -1, CLASS_TYPE);

        succ = self->Bind(ip, port);

        printf("Bind: %d\n", succ);

        return ret;
    }
    else
    {
        TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);
        
        ip = luaL_checkstring(L, 2);
	    port = luaL_checkinteger(L, 3);

        succ = self->Bind(ip, port);

        printf("succ %d\n", succ);
    
        return succ;
    }

	return 0;
}

int tcpReceive(lua_State * L)
{
    char buffer[SOCKET_BUFFERSIZE];
    
    TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);
    self->Receive(buffer);

    lua_pushstring(L, buffer);
    lua_pushnil(L);

    return 2;
}

int tcpSend(lua_State * L)
{
    TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    size_t stringLength;
    string datagram = luaL_checklstring(L, 2, &stringLength);
    
    int sent = self->Send(datagram.c_str(), stringLength);

    return sent;
}

int tcpSetTimeout(lua_State * L)
{
    return 0;
}

int tcpClose(lua_State * L)
{
    TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    self->Close();

    return 0;
}

int tcpGC(lua_State * L)
{
	int ret = tcpClose(L);

    return ret;
}

int tcpToString(lua_State * L)
{
	TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    char * data = self->ToString();

    lua_pushstring(L, data);

    free(data);

    return 1;
}

int initTCPClass(lua_State * L)
{
	luaL_Reg reg[] = 
    {
        { "__gc",        tcpGC          },
        { "__tostring",  tcpToString    },
        { "accept",      tcpAccept      },
        { "bind",        tcpBind        },
        { "close",       tcpClose       },
        { "getsockname", tcpGetSockName },
        { "new",		 tcpNew         },
        { "receive",     tcpReceive     },
        { "send",        tcpSend        },
        { "settimeout",  tcpSetTimeout  },
        { 0, 0 },
    };

    luaobj_newclass(L, CLASS_NAME, NULL, tcpNew, reg);

    return 1;
}