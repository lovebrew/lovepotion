#include "common/runtime.h"
#include "socket/objects/socket/wrap_socket.h"

using namespace love;

#define CLASS_NAME "Socket"
#define CLASS_TYPE LUAOBJ_TYPE_SOC

int Wrap_Socket::New(lua_State * L)
{
    void * raw_self = luaobj_newudata(L, sizeof(Socket));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    new (raw_self) Socket();

    return 1;
}

int Wrap_Socket::GetPeerName(lua_State * L)
{
    Socket * self = Wrap_Socket::CheckSocket(L, 1);

    Socket::Address address;
    int error = self->GetPeerName(address);

    if (error != 0)
    {
        lua_pushnil(L);

        if (error == -1)
            lua_pushstring(L, Socket::GetError(errno));
        else
            lua_pushstring(L, gai_strerror(error));

        return 2;
    }

    lua_pushlstring(L, address.ip.data(), address.ip.size());
    lua_pushnumber(L, std::stol(address.port));

    return 2;
}

int Wrap_Socket::GetSockName(lua_State * L)
{
    Socket * self = Wrap_Socket::CheckSocket(L, 1);

    Socket::Address address;
    int error = self->GetSockName(address);

    if (error != 0)
    {
        lua_pushnil(L);

        if (error == -1)
            lua_pushstring(L, Socket::GetError(errno));
        else
            lua_pushstring(L, gai_strerror(error));

        return 2;
    }

    lua_pushlstring(L, address.ip.data(), address.ip.size());
    lua_pushnumber(L, std::stol(address.port));

    return 2;
}

int Wrap_Socket::SetTimeout(lua_State * L)
{
    Socket * self = Wrap_Socket::CheckSocket(L, 1);

    double duration = luaL_optnumber(L, 2, -1);
    const char * mode = luaL_optstring(L, 3, "b");

    bool success = self->SetTimeout(mode, duration);

    if (!success)
        luaL_argcheck(L, 0, 3, "invalid timeout mode");

    lua_pushnumber(L, 1);

    return 1;
}

int Wrap_Socket::Close(lua_State * L)
{
    Socket * self = Wrap_Socket::CheckSocket(L, 1);

    self->Destroy();

    lua_pushnumber(L, 1);

    return 1;
}

int Wrap_Socket::GarbageCollect(lua_State * L)
{
    int ret = Wrap_Socket::Close(L);

    return ret;
}

Socket * Wrap_Socket::CheckSocket(lua_State * L, int index)
{
    return (Socket *)luaobj_checkudata(L, index, CLASS_TYPE);
}

int Wrap_Socket::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "__gc",        GarbageCollect },
        { "getpeername", GetPeerName    },
        { "getsockname", GetSockName    },
        { "close",       Close          },
        { "settimeout",  SetTimeout     },
        { 0,             0              }
    };

    return luaobj_newclass(L, CLASS_NAME, NULL, Wrap_Socket::New, reg);
}
