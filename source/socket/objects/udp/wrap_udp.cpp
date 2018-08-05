#include "common/runtime.h"
#include "socket/common.h"

#include "socket/objects/socket.h"

#include "socket/objects/udp/udp.h"
#include "socket/objects/udp/wrap_udp.h"

#define CLASS_NAME "UDP"
#define CLASS_TYPE LUAOBJ_TYPE_UDP

int udpNew(lua_State * L)
{
    void * raw_self = luaobj_newudata(L, sizeof(UDP));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    UDP * self = new (raw_self) UDP();

    return 1;
}

//UDP:send
int udpSend(lua_State * L)
{
    UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    size_t length = 0;
    const char * datagram = luaL_checklstring(L, 2, &length);

    int sent = self->Send(datagram, length);

    if (sent < 0)
        lua_pushnil(L);
    else
        lua_pushinteger(L, sent);

    return 1;
}


//UDP:sendto
int udpSendTo(lua_State * L)
{
    UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    size_t length = 0;
    const char * datagram = luaL_checklstring(L, 2, &length);

    const char * ip = luaL_checkstring(L, 3);
    int port = luaL_checknumber(L, 4);

    int sent = self->SendTo(datagram, length, ip, port);

    if (sent < 0)
        lua_pushnil(L);
    else
        lua_pushinteger(L, sent);

    return 1;
}

//UDP:setpeername
int udpSetPeerName(lua_State * L)
{
    UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    string ip = luaL_checkstring(L, 2);
    int port = luaL_checknumber(L, 3);

    int status = self->SetPeerName(ip, port);

    lua_pushinteger(L, status);

    return 1;
}

//UDP:setsockname
int udpSetSockName(lua_State * L)
{
    UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    string ip = luaL_checkstring(L, 2);
    int port = luaL_checknumber(L, 3);

    int status = self->SetSockName(ip, port);

    lua_pushinteger(L, status);

    return 1;
}

//UDP:receivefrom
int udpReceiveFrom(lua_State * L)
{
    UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    Datagram datagram;

    datagram.buffer = (char *)calloc(SOCKET_BUFFERSIZE, sizeof(char));
    datagram.ip = (char *)calloc(0x40, sizeof(char));

    int length = self->ReceiveFrom(datagram);

    if (length == 0)
    {
        lua_pushnil(L);
        return 1;
    }
    
    lua_pushstring(L, datagram.buffer);
    lua_pushstring(L, datagram.ip);
    lua_pushinteger(L, datagram.port);

    return 3;
}

//UDP:receive
int udpReceive(lua_State * L)
{
    UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);
    size_t bytes = luaL_optnumber(L, 2, -1);

    char buffer[SOCKET_BUFFERSIZE];
    
    int length = self->Receive(buffer, NULL, bytes);

    if (length == 0)
    {
        lua_pushnil(L);
        return 1;
    }

    lua_pushstring(L, buffer);

    return 1;
}

int udpClose(lua_State * L)
{
    UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    self->Close();

    return 0;
}

int udpGC(lua_State * L)
{
    int ret = udpClose(L);

    return ret;
}

int udpToString(lua_State * L)
{
    UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    char * data = self->ToString();

    lua_pushstring(L, data);

    free(data);

    return 1;
}

int initUDPClass(lua_State * L) 
{
    luaL_Reg reg[] = 
    {
        { "__gc",        udpGC          },
        { "__tostring",  udpToString    },
        { "close",       udpClose       },
        { "new",         udpNew         },
        { "receive",     udpReceive     },
        { "receivefrom", udpReceiveFrom },
        { "send",        udpSend        },
        { "sendto",      udpSendTo      },
        { "setpeername", udpSetPeerName },
        { "setsockname", udpSetSockName },
        { 0, 0 },
    };

    luaobj_newclass(L, CLASS_NAME, NULL, udpNew, reg);

    return 1;
}