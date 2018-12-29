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

    new (raw_self) TCP();

    return 1;
}

int tcpBind(lua_State * L)
{
    if (lua_type(L, 1) != LUA_TUSERDATA)
    {
        string ip = luaL_checkstring(L, 1);
        int port = luaL_checknumber(L, 2);

        tcpNew(L);

        TCP * self = (TCP *)luaobj_checkudata(L, -1, CLASS_TYPE);

        int status = self->Bind(ip, port);

        return status;
    }
    else
    {
        TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);

        string ip = luaL_checkstring(L, 2);
        int port = luaL_checknumber(L, 3);

        int status = self->Bind(ip, port);

        return status;
    }

    return 0;
}

int tcpConnect(lua_State * L)
{
    TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    string ip = luaL_checkstring(L, 2);
    int port = luaL_checknumber(L, 3);

    int status = self->Connect(ip, port);

    lua_pushinteger(L, status);

    return 1;
}

int tcpOnAccept(lua_State * L, int newfd)
{
    void * raw_self = luaobj_newudata(L, sizeof(TCP));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    new (raw_self) TCP(newfd);

    return 1;
}

int tcpAccept(lua_State * L)
{
    TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    int newfd = self->Accept();
    
    if (newfd < 0)
    {
        lua_pushnil(L);
        
        return 1;
    }

    int succ = tcpOnAccept(L, newfd);

    return succ;
}

int tcpReceive(lua_State * L)
{
    TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    string pattern = luaL_optstring(L, 2, "*l");
    string prefix = luaL_optstring(L, 3, "");

    char * buffer = nullptr;
    int recvd = 0;

    if (pattern == "*l")
        recvd = self->ReceiveLines(&buffer);

    if (recvd < 0)
    {
        lua_pushnil(L);

        if (recvd == -1)
            lua_pushstring(L, "closed");
        else if (recvd == -2)
            lua_pushstring(L, "timeout");

        return 2;
    }
    else if (recvd == 0)
        lua_pushstring(L, "");
    else
        lua_pushstring(L, buffer);

    LOG("buffer is %s", buffer);

    if (buffer != nullptr)
        free(buffer);

    lua_pushnil(L);

    return 2;
}

int tcpSend(lua_State * L)
{
    TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);
    size_t stringLength = 0;

    LOG("get datagram");
    const char * datagram = luaL_checklstring(L, 2, &stringLength);

    long start = luaL_optnumber(L, 3, 1);
    long end   = luaL_optnumber(L, 4, -1);

    if (start < 0)
        start = (long)(stringLength + start + 1);

    if (end < 0)
        end = (long)(stringLength + end + 1);

    if (start < 1)
        start = (long)1;

    if (end < (long)stringLength)
        end = (long)stringLength;

    LOG("Preparing to send?");

    int sent = 0;
    if (start <= end)
        sent = self->Send(datagram + start - 1, end - start + 1);
    LOG("Sent %d!", sent);

    if (sent < 0)
    {
        lua_pushnil(L);

        if (sent == -1)
            lua_pushstring(L, "closed");
        else if (sent == -2)
            lua_pushstring(L, "timeout");

        return 2;
    }

    lua_pushnumber(L, sent + start + 1);
    lua_pushnil(L);

    return 2;
}

//TCP:getsockname
int tcpGetSockName(lua_State * L)
{
    TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    char origin[0x40];
    int port;

    int error = self->GetSockName(origin, &port);

    if (error < 0)
    {
        lua_pushnil(L);

        return 1;
    }

    lua_pushstring(L, origin);
    lua_pushinteger(L, port);

    return 2;
}

int tcpGetPeerName(lua_State * L)
{
    TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    char origin[0x40];
    int port;

    int error = self->GetPeerName(origin, &port);

    if (error < 0)
    {
        lua_pushnil(L);

        return 1;
    }

    lua_pushstring(L, origin);
    lua_pushinteger(L, port);

    return 2;
}

int tcpSetTimeout(lua_State * L)
{
    TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    double timeout = luaL_optnumber(L, 2, -1);

    self->SetTimeout(timeout);

    return 0;
}

int tcpClose(lua_State * L)
{
    TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    self->Close();

    return 0;
}

int initTCPClass(lua_State * L)
{
	luaL_Reg reg[] = 
    {
        //{ "__gc",        tcpGC          },
        //{ "__tostring",  tcpToString    },
        { "accept",      tcpAccept      },
        { "bind",        tcpBind        },
        { "close",       tcpClose       },
        { "getpeername", tcpGetPeerName },
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

/*int tcpGetSockName(lua_State * L)
{
    char ip[0x40];
    int port = 0;

    TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);
    self->GetSockName(ip, port);

    lua_pushstring(L, ip);
    lua_pushnumber(L, port);

    return 2;
}

int tcpBind(lua_State * L)
{
    int succ;
    
    string ip;
    int port;

    //socket.bind
    if (lua_type(L, 1) != LUA_TUSERDATA)
	{
        ip = luaL_checkstring(L, 1);
	    port = luaL_checkinteger(L, 2);

        int ret = tcpNew(L);

        TCP * self = (TCP *)luaobj_checkudata(L, -1, CLASS_TYPE);

        succ = self->Bind(ip, port);
        if (succ)
            self->Listen(); //server

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
    const char * pattern = luaL_optstring(L, 2, "*l");
    size_t bytes = luaL_optnumber(L, 3, SOCKET_BUFFERSIZE);

    self->Receive(buffer, pattern, bytes);

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
}*/