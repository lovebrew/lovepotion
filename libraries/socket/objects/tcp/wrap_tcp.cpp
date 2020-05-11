#include "common/runtime.h"
#include "socket/objects/tcp/wrap_tcp.h"

#define CLASS_TYPE LUAOBJ_TYPE_TCP
#define CLASS_NAME "TCP"

int Wrap_TCP::New(lua_State * L)
{
    void * raw_self = luaobj_newudata(L, sizeof(TCP));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    int success;

    TCP * self = new (raw_self) TCP(success, AF_UNSPEC);

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

int Wrap_TCP::New4(lua_State * L)
{
    void * raw_self = luaobj_newudata(L, sizeof(TCP));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    int success;

    TCP * self = new (raw_self) TCP(success, AF_INET);

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

    TCP * self = Wrap_TCP::CheckTCPSocket(L, 1);
    int clientfd;

    const char * error = self->TryAccept(&clientfd);

    if (error)
    {
        lua_pushnil(L);
        lua_pushstring(L, error);

        return 2;
    }
    else
    {
        void * raw_self = luaobj_newudata(L, sizeof(TCP));

        luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

        TCP * client = new (raw_self) TCP(clientfd);
        client->SetNonBlocking();

        return 1;
    }
}

int Wrap_TCP::Bind(lua_State * L)
{
    if (!Wrap_TCP::CheckTCPSocketType(L, "master", 1))
        return luaL_argerror(L, 1, "tcp{master} expected");

    TCP * self = Wrap_TCP::CheckTCPSocket(L, 1);

    Socket::Address host;

    host.ip = luaL_checkstring(L, 2);
    host.port = luaL_checkstring(L, 3);

    const char * error = self->TryBind(SOCK_STREAM, host);

    if (error)
    {
        lua_pushnil(L);
        lua_pushstring(L, error);

        return 2;
    }

    lua_pushnumber(L, 1);

    return 1;
}

int Wrap_TCP::Connect(lua_State * L)
{
    if (!Wrap_TCP::CheckTCPSocketType(L, "master", 1))
        return luaL_argerror(L, 1, "tcp{master} expected");

    TCP * self = Wrap_TCP::CheckTCPSocket(L, 1);

    self->SetTimeoutStart();

    Socket::Address peer;

    peer.ip = luaL_checkstring(L, 2);
    peer.port = luaL_checkstring(L, 3);

    const char * error = self->TryConnect(SOCK_STREAM, peer);

    if (error)
    {
        lua_pushnil(L);
        lua_pushstring(L, error);

        return 2;
    }

    self->SetState(TCP::STATE_CLIENT);

    lua_pushnumber(L, 1);

    return 1;
}

int Wrap_TCP::GetPeerName(lua_State * L)
{
    if (!Wrap_TCP::CheckTCPSocketType(L, "client", 1))
        return luaL_argerror(L, 1, "tcp{client} expected");

    return Wrap_Socket::GetPeerName(L);
}

int Wrap_TCP::Listen(lua_State * L)
{
    if (!Wrap_TCP::CheckTCPSocketType(L, "master", 1))
        return luaL_argerror(L, 1, "tcp{master} expected");

    TCP * self = Wrap_TCP::CheckTCPSocket(L, 1);

    int backlog = luaL_optnumber(L, 2, TCP::DEFAULT_BACKLOG);

    int error = self->Listen(backlog);

    if (error != IO::IO_DONE)
    {
        lua_pushnil(L);
        lua_pushstring(L, Socket::GetError(error));

        return 2;
    }

    self->SetState(TCP::STATE_SERVER);

    lua_pushnumber(L, 1);

    return 1;
}

int Wrap_TCP::Receive(lua_State * L)
{
    if (!Wrap_TCP::CheckTCPSocketType(L, "client", 1))
        return luaL_argerror(L, 1, "tcp{client} expected");

    TCP * self = Wrap_TCP::CheckTCPSocket(L, 1);

    int top = lua_gettop(L);
    int error = IO::IO_DONE;

    size_t size = 0;
    luaL_Buffer buff;

    const char * part = luaL_optlstring(L, 3, "", &size);
    self->SetBufferTimeoutStart();

    luaL_buffinit(L, &buff);
    luaL_addlstring(&buff, part, size);

    if (!lua_isnumber(L, 2))
    {
        std::string mode = luaL_optstring(L, 2, "*l");

        if (mode == "*l")
            error = self->ReceiveLine(&buff);
        else if (mode == "*a")
            error = self->ReceiveAll(&buff);
        else
            luaL_argcheck(L, 0, 2, "invalid receive pattern");
    }
    else
    {
        double max = lua_tonumber(L, 2);
        size_t wanted = (size_t)max;

        luaL_argcheck(L, max >= 0, 2, "invalid receive pattern");

        if (size == 0 || wanted > size)
            error = self->ReceiveRaw(wanted - size, &buff);
    }

    if (error != IO::IO_DONE)
    {
        luaL_pushresult(&buff);
        lua_pushstring(L, Socket::GetError(error));
        lua_pushvalue(L, -2);
        lua_pushnil(L);
        lua_replace(L, -4);
    }
    else
    {
        luaL_pushresult(&buff);
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return lua_gettop(L) - top;
}

int Wrap_TCP::Send(lua_State * L)
{
    if (!Wrap_TCP::CheckTCPSocketType(L, "client", 1))
        return luaL_argerror(L, 1, "tcp{client} expected");

    TCP * self = Wrap_TCP::CheckTCPSocket(L, 1);
    int top = lua_gettop(L);

    int error = IO::IO_DONE;
    size_t size = 0;
    size_t sent = 0;

    const char * data = luaL_checklstring(L, 2, &size);
    long start = luaL_optnumber(L, 3, 1);
    long end   = luaL_optnumber(L, 4, -1);

    self->SetBufferTimeoutStart();

    if (start < 0)
        start = (long)(size + start + 1);

    if (end < 0)
        end   = (long)(size + end + 1);

    if (start < 1)
        start = 1;

    if (end > (long)size)
        end = (long)size;

    if (start <= end)
        error = self->SendRaw(data + start - 1, end - start + 1, &sent);

    if (error != IO::IO_DONE)
    {
        lua_pushnil(L);
        lua_pushstring(L, Socket::GetError(error));
        lua_pushnumber(L, (sent + start - 1));
    }
    else
    {
        lua_pushnumber(L, (sent + start - 1));
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return lua_gettop(L) - top;
}

int Wrap_TCP::Shutdown(lua_State * L)
{
    if (!Wrap_TCP::CheckTCPSocketType(L, "client", 1))
        return luaL_argerror(L, 1, "tcp{client} expected");

    TCP * self = Wrap_TCP::CheckTCPSocket(L, 1);

    std::string how = luaL_optstring(L, 2, "both");

    self->Shutdown(how);

    lua_pushnumber(L, 1);

    return 1;
}

int Wrap_TCP::SetOption(lua_State * L)
{
    TCP * self = Wrap_TCP::CheckTCPSocket(L, 1);

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

    static const std::string types[] = {"{master}", "{client}", "{server}"};
    std::string selfType = ret->GetString();
    bool success = false;

    for (auto item : types)
    {
        if (item == "{" + type + "}" && selfType.find(item) != std::string::npos)
        {
            success = true;
            break;
        }
    }

    return success;
}

int Wrap_TCP::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "__tostring",  ToString    },
        { "bind",        Bind        },
        { "connect",     Connect     },
        { "setoption",   SetOption   },
        { "send",        Send        },
        { "receive",     Receive     },
        { "accept",      Accept      },
        { "listen",      Listen      },
        { "shutdown",    Shutdown    },
        { "getpeername", GetPeerName },
        { 0, 0 },
    };

    return luaobj_newclass(L, CLASS_NAME, "Socket", Wrap_TCP::New, reg);
}
