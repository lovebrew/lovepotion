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

    if (success != IO::IO_DONE)
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

    if (connecting && self->IsConnected())
        self->TryDisconnect();

    const char * error = self->TryConnect(SOCK_DGRAM, peer);

    if (error)
    {
        lua_pushnil(L);
        lua_pushstring(L, error);

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

    const char * error = self->TryBind(SOCK_DGRAM, host);

    if (error)
    {
        lua_pushnil(L);
        lua_pushstring(L, error);

        return 2;
    }

    lua_pushnumber(L, 1);

    return 1;
}

int Wrap_UDP::Receive(lua_State * L)
{
    UDP * self = Wrap_UDP::CheckUDPSocket(L, 1);

    char buffer[UDP_DATASIZE];
    size_t got, wanted = luaL_optnumber(L, 2, sizeof(buffer));

    char * data = wanted > sizeof(buffer) ? (char *)malloc(wanted) : buffer;
    self->SetTimeoutStart();

    if (!data)
    {
        lua_pushnil(L);
        lua_pushliteral(L, "out of memory");

        return 2;
    }

    int error = self->Receive(data, wanted, &got, &self->GetTimeout());

    if (error != IO::IO_DONE && error != IO::IO_CLOSED)
    {
        lua_pushnil(L);
        lua_pushstring(L, Socket::GetError(error));

        if (wanted > sizeof(buffer))
            free(data);

        return 2;
    }

    lua_pushlstring(L, data, got);

    if (wanted > sizeof(buffer))
        free(data);

    return 1;
}

int Wrap_UDP::ReceiveFrom(lua_State * L)
{
    if (Wrap_UDP::CheckUDPSocketType(L, "connected", 1))
        return luaL_argerror(L, 1, "udp{unconnected} expected");

    UDP * self = Wrap_UDP::CheckUDPSocket(L, 1);

    char buffer[UDP_DATASIZE];
    size_t got, wanted = luaL_optnumber(L, 2, sizeof(buffer));

    char * data = wanted > sizeof(buffer) ? (char *)malloc(wanted) : buffer;
    self->SetTimeoutStart();

    if (!data)
    {
        lua_pushnil(L);
        lua_pushstring(L, "out of memory");

        return 2;
    }

    Socket::Address address;

    int error = self->ReceiveFrom(buffer, wanted, &got, address);

    if (error)
    {
        lua_pushnil(L);
        lua_pushstring(L, Socket::GAIError(error));

        if (wanted > sizeof(buffer))
            free(data);

        return 2;
    }

    if (error != IO::IO_DONE && error != IO::IO_CLOSED)
    {
        lua_pushnil(L);
        lua_pushstring(L, Socket::GetError(error));

        return 2;
    }

    lua_pushlstring(L, data, got);
    lua_pushlstring(L, address.ip.data(), address.ip.size());
    lua_pushinteger(L, std::stol(address.port));

    if (wanted > sizeof(buffer))
        free(data);

    return 3;
}

int Wrap_UDP::Send(lua_State * L)
{
    if (CheckUDPSocketType(L, "unconnected", 1))
        return luaL_argerror(L, 1, "udp{connected} expected");

    UDP * self = Wrap_UDP::CheckUDPSocket(L, 1);

    size_t sent = 0;
    size_t length = 0;

    const char * data = luaL_checklstring(L, 2, &length);

    self->SetTimeoutStart();

    int error = self->Send(data, length, &sent, &self->GetTimeout());

    if (error != IO::IO_DONE)
    {
        lua_pushnil(L);

        std::string msg = Socket::GetError(error);
        if (error == IO::IO_CLOSED)
             msg = "refused";

        lua_pushlstring(L, msg.data(), msg.size());

        return 2;
    }

    lua_pushnumber(L, sent);

    return 1;
}

int Wrap_UDP::SendTo(lua_State * L)
{
    if (CheckUDPSocketType(L, "connected", 1))
        return luaL_argerror(L, 1, "udp{unconnected} expected");

    UDP * self = Wrap_UDP::CheckUDPSocket(L, 1);

    Socket::Address address;
    size_t length;
    size_t sent = 0;

    const char * data = luaL_checklstring(L, 2, &length);
    address.ip = luaL_checkstring(L, 3);
    address.port = luaL_checkstring(L, 4);

    self->SetTimeoutStart();

    int error = self->SendTo(data, length, &sent, address);

    if (error)
    {
        lua_pushnil(L);
        lua_pushstring(L, gai_strerror(error));

        return 2;
    }

    if (error != IO::IO_DONE)
    {
        lua_pushnil(L);

        std::string msg = Socket::GetError(error);
        if (error == IO::IO_CLOSED)
             msg = "refused";

        lua_pushlstring(L, msg.data(), msg.size());

        return 2;
    }

    lua_pushnumber(L, sent);

    return 1;
}

int Wrap_UDP::SetOption(lua_State * L)
{
    UDP * self = Wrap_UDP::CheckUDPSocket(L, 1);

    std::string option = luaL_checkstring(L, 2);
    int value = lua_toboolean(L, 3);

    std::string error = self->SetOption(option, value);

    if (!error.empty())
    {
        lua_pushnil(L);
        lua_pushlstring(L, error.data(), error.size());

        return 2;
    }

    lua_pushnumber(L, 1);

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

    static const std::string types[] = {"{unconnected}", "{connected}"};
    std::string selfType = ret->GetString();

    for (auto item : types)
    {
        if (item == "{" + type + "}")
            return selfType.find(item) != std::string::npos;
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
        { "sendto",      SendTo      },
        { "receive",     Receive     },
        { "receivefrom", ReceiveFrom },
        { "setoption",   SetOption   },
        { 0, 0 },
    };

    return luaobj_newclass(L, CLASS_NAME, "Socket", Wrap_UDP::New, reg);
}