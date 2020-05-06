#include "common/runtime.h"
#include "socket/objects/udp/wrap_udp.h"

#define CLASS_NAME "UDP"
#define CLASS_TYPE LUAOBJ_TYPE_UDP

int Wrap_UDP::New(lua_State * L)
{
    void * raw_self = luaobj_newudata(L, sizeof(UDP));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    int success;

    UDP * self = new (raw_self) UDP(success);

    if (success != IO_DONE)
    {
        self->~UDP();
        free(raw_self);

        lua_pushnil(L);
        lua_pushstring(L, Socket::GetError(success));

        return 2;
    }

    return 1;
}

int Wrap_UDP::GetPeerName(lua_State * L)
{
    if (Wrap_UDP::CheckUDPSocketType(L, "connected", 1))
        return Wrap_Socket::GetPeerName(L);

    return luaL_argerror(L, 1, "udp{connected} expected");
}

int Wrap_UDP::SetPeerName(lua_State * L)
{
    UDP * self = Wrap_UDP::CheckUDPSocket(L, 1);

    Socket::Address peer;

    peer.ip = luaL_checkstring(L, 2);

    bool connecting = (peer.ip != "*");
    peer.port = connecting ? luaL_checkstring(L, 3) : "0";

    int error = self->SetPeerName(peer);

    if (error != 0)
    {
        lua_pushnil(L);

        if (error)
            lua_pushstring(L, Socket::GAIError(error));
        else
            lua_pushstring(L, Socket::GetError(errno));

        return 2;
    }

    lua_pushnumber(L, 1);

    return 1;
}

int Wrap_UDP::SetSockName(lua_State * L)
{
    if (Wrap_UDP::CheckUDPSocketType(L, "connected", 1))
        return luaL_argerror(L, 1, "udp{unconnected} expected");

    UDP * self = Wrap_UDP::CheckUDPSocket(L, 1);

    Socket::Address host;

    host.ip = luaL_checkstring(L, 2);
    host.port = luaL_optstring(L, 3, "0");

    int error = self->SetSockName(host);

    if (error != 0)
    {
        lua_pushnil(L);

        if (error)
            lua_pushstring(L, Socket::GAIError(error));
        else
            lua_pushstring(L, Socket::GetError(errno));

        return 2;
    }

    lua_pushnumber(L, 1);

    return 1;
}

int Wrap_UDP::Send(lua_State * L)
{
    if (CheckUDPSocketType(L, "unconnected", 1))
        return luaL_argerror(L, 1, "udp{connected} expected");

    UDP * self = Wrap_UDP::CheckUDPSocket(L, 1);

    size_t sent = 0;
    size_t length = 0;

    const char * data = luaL_checklstring(L, 2, &length);

    int error = self->Send(data, length, &sent);

    if (error != IO_DONE)
    {
        lua_pushnil(L);

        std::string msg = Socket::GetError(error);
        if (error == IO_CLOSED)
             msg = "refused";

        lua_pushlstring(L, msg.data(), msg.size());

        return 2;
    }

    lua_pushnumber(L, sent);

    return 1;
}

int Wrap_UDP::ToString(lua_State * L)
{
    UDP * self = Wrap_UDP::CheckUDPSocket(L, 1);

    std::string str = self->GetString();
    lua_pushlstring(L, str.data(), str.size());

    return 1;
}

UDP * Wrap_UDP::CheckUDPSocket(lua_State * L, int index)
{
    return (UDP *)luaobj_checkudata(L, index, CLASS_TYPE);
}

bool Wrap_UDP::CheckUDPSocketType(lua_State * L, const std::string & type, int index)
{
    UDP * ret = Wrap_UDP::CheckUDPSocket(L, index);

    static const std::string types[] = {"{unconnected}", "{connected}", ""};
    std::string selfType = ret->GetString();

    for (size_t i = 0; !(types[i].empty()); i++)
    {
        if (types[i].compare(type) == 0)
            return selfType.find(types[i]) != std::string::npos;
    }

    return false;
}

int Wrap_UDP::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "__tostring",  ToString    },
        { "setpeername", SetPeerName },
        { "getpeername", GetPeerName },
        { "setsockname", SetSockName },
        { "send",        Send        },
        // { "setsockname", udpSetSockName },
        // { "settimeout",  udpSetTimeout  },
        { 0, 0 },
    };

    return luaobj_newclass(L, CLASS_NAME, "Socket", Wrap_UDP::New, reg);
}
