#include "socket/luasocket.h"

int LuaSocket::Open(lua_State * L)
{
    int (*classes[])(lua_State *L) =
    {
        Wrap_Socket::Register,
        Wrap_UDP::Register,
        NULL,
    };

    for (size_t i = 0; classes[i]; i++)
    {
        classes[i](L);
        lua_pop(L, 1);
    }

    luaL_Reg reg[] =
    {
        { "udp", Wrap_UDP::New },
        { 0, 0 },
    };

    luaL_newlib(L, reg);

    return 1;
}

int LuaSocket::OpenHTTP(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "request", Wrap_HTTP::Request },
        { 0, 0 },
    };

    luaL_newlib(L, reg);

    return 1;
}
