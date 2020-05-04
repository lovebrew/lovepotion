#include "common/runtime.h"
#include "socket/objects/socket/wrap_socket.h"

using namespace love;

int Wrap_Socket::GetPeerName(lua_State * L)
{
    // Socket * self = Wrap_Socket::CheckSocket(L, 1);

    // Socket::Address address;
    // self->GetPeerName(address);

    // lua_pushlstring(L, address.ip.data(), address.ip.size());
    // lua_pushlstring(L, address.port.data(), address.port.size());

    // return 2;
}

int Wrap_Socket::Close(lua_State * L)
{
    // Socket * self = Wrap_Socket::CheckSocket(L, 1);

    // self->Close();

    // return 0;
}

// luaL_Reg Wrap_Socket::functions[5] =
// {
//     { "getpeername", GetPeerName         },
//     { "__tostring",  LuaSocket::ToString },
//     { "close",       Close               },
//     { 0,             0                   }
// };

int Wrap_Socket::Register(lua_State * L)
{
    // LuaSocket::
}
