#include "common/runtime.h"
#include "socket/objects/tcp/wrap_tcp.h"

#define CLASS_TYPE LUAOBJ_TYPE_TCP
#define CLASS_NAME "TCP"

int Wrap_TCP::New(lua_State * L)
{
    void * raw_self = luaobj_newudata(L, sizeof(TCP));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    int success;

    TCP * self = new (raw_self) TCP(success);

    if (success != IO::IO_DONE)
    {
        self->~TCP();
        free(raw_self);

        lua_pushnil(L);
        lua_pushstring(L, Socket::GetError(success));

        return 2;
    }

    return 1;
}

int Wrap_TCP::Accept(lua_State * L)
{
    if (!Wrap_TCP::CheckTCPSocketType(L, "server", 1))
        return luaL_argerror(L, 1, "tcp{server} expected");
}

int Wrap_TCP::Bind(lua_State * L)
{
    if (!Wrap_TCP::CheckTCPSocketType(L, "master", 1))
        return luaL_argerror(L, 1, "tcp{master} expected");
}

int Wrap_TCP::Connect(lua_State * L)
{
    if (!Wrap_TCP::CheckTCPSocketType(L, "master", 1))
        return luaL_argerror(L, 1, "tcp{master} expected");
}

int Wrap_TCP::GetPeerName(lua_State * L)
{
    if (!Wrap_TCP::CheckTCPSocketType(L, "client", 1))
        return luaL_argerror(L, 1, "tcp{client} expected");
}

int Wrap_TCP::Listen(lua_State * L)
{
    if (!Wrap_TCP::CheckTCPSocketType(L, "master", 1))
        return luaL_argerror(L, 1, "tcp{master} expected");
}

int Wrap_TCP::Receive(lua_State * L)
{
    if (!Wrap_TCP::CheckTCPSocketType(L, "client", 1))
        return luaL_argerror(L, 1, "tcp{client} expected");
}

int Wrap_TCP::Send(lua_State * L)
{
    if (!Wrap_TCP::CheckTCPSocketType(L, "client", 1))
        return luaL_argerror(L, 1, "tcp{client} expected");
}

int Wrap_TCP::Shutdown(lua_State * L)
{
    if (!Wrap_TCP::CheckTCPSocketType(L, "client", 1))
        return luaL_argerror(L, 1, "tcp{client} expected");
}

int Wrap_TCP::SetOption(lua_State * L)
{
    if (Wrap_TCP::CheckTCPSocketType(L, "master", 1))
        return luaL_argerror(L, 1, "tcp{client} or tcp{server} expected");
}

int Wrap_TCP::ToString(lua_State * L)
{
    TCP * self = Wrap_TCP::CheckTCPSocket(L, 1);

    std::string str = self->GetString();
    lua_pushlstring(L, str.data(), str.size());

    return 1;
}

TCP * Wrap_TCP::CheckTCPSocket(lua_State * L, int index)
{
    return (TCP *)luaobj_checkudata(L, index, CLASS_TYPE);
}

bool Wrap_TCP::CheckTCPSocketType(lua_State * L, const std::string & type, int index)
{
    TCP * ret = Wrap_TCP::CheckTCPSocket(L, index);

    static const std::string types[] = {"{master}", "{client}", "server"};
    std::string selfType = ret->GetString();

    for (auto check : types)
    {
        if (check == type)
            return selfType.find(check) != std::string::npos;
    }

    return false;
}

int Wrap_TCP::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "__tostring",  ToString    },
        { 0, 0 },
    };

    return luaobj_newclass(L, CLASS_NAME, "Socket", Wrap_TCP::New, reg);
}
