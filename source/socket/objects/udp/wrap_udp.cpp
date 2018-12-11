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

    new (raw_self) UDP();

    return 1;
}

//UDP:send
int udpSend(lua_State * L)
{
    UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    size_t length = 0;
    const char * datagram = luaL_checklstring(L, 2, &length);

    string value = self->GetIP();
    const char * destination = value.c_str();

    int sent = self->SendTo(datagram, length, destination, self->GetPort());

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
    size_t bytes = luaL_optnumber(L, 2, SOCKET_BUFFERSIZE);

    char buffer[bytes + 1];
    char origin[0x40];
    int port;

    int length = self->ReceiveFrom(buffer, origin, &port, bytes);

    if (length == 0)
    {
        lua_pushnil(L);
        return 1;
    }
    
    lua_pushstring(L, buffer);
    lua_pushstring(L, origin);
    lua_pushinteger(L, port);

    return 3;
}

//UDP:receive
int udpReceive(lua_State * L)
{
    UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);
    size_t bytes = luaL_optnumber(L, 2, SOCKET_BUFFERSIZE);

    char buffer[bytes + 1];
    int length = self->ReceiveFrom(buffer, NULL, NULL, bytes);

    if (length == 0)
        lua_pushnil(L);
    else
        lua_pushstring(L, buffer);

    return 1;
}

int udpClose(lua_State * L)
{
    UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    self->Close();

    return 0;
}

int udpSetTimeout(lua_State * L)
{
    UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    double timeout = luaL_checknumber(L, 2);

    self->SetTimeout(timeout);

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
        { "settimeout",  udpSetTimeout  },
        { 0, 0 },
    };

    luaobj_newclass(L, CLASS_NAME, NULL, udpNew, reg);

    return 1;
}