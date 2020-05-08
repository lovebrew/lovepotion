#include "socket/libsocket.h"
#include "socket_lua.h"

#define LUASOCKET_TIME   1000000000
#define OH_GOD_BIG_VALUE 999999999

int LuaSocket::Sleep(lua_State * L)
{
    double value = luaL_checknumber(L, 1);
    timespec time, remainder;

    if (value < 0.0)
        value = 0.0;
    else if (value > std::numeric_limits<int>::max())
        value = std::numeric_limits<int>::max();

    time.tv_sec = (int)value;
    value -= time.tv_sec;
    time.tv_nsec = (int)(value * LUASOCKET_TIME);

    if (time.tv_nsec >= LUASOCKET_TIME)
        time.tv_nsec = OH_GOD_BIG_VALUE;

    while (nanosleep(&time, &remainder) != 0)
    {
        time.tv_sec = remainder.tv_sec;
        time.tv_nsec = remainder.tv_nsec;
    }

    return 0;
}

int LuaSocket::OpenSocket(lua_State * L)
{
    if (Luax::DoBuffer(L, (const char *)socket_lua, socket_lua_size, "socket.lua"))
        LOG("socket.lua error: %s", lua_tostring(L, -1));

    return 1;
}

int LuaSocket::Open(lua_State * L)
{
    Luax::Preload(L, LuaSocket::OpenCore, "socket.core");

    Luax::Preload(L, LuaSocket::OpenSocket, "socket");
    Luax::Preload(L, LuaSocket::OpenHTTP, "socket.http");

    return 0;
}

int LuaSocket::OpenCore(lua_State * L)
{
    int (*classes[])(lua_State *L) =
    {
        Wrap_Socket::Register,
        Wrap_UDP::Register,
        Wrap_TCP::Register,
        NULL,
    };

    for (size_t i = 0; classes[i]; i++)
    {
        classes[i](L);
        lua_pop(L, 1);
    }

    luaL_Reg reg[] =
    {
        { "udp",   Wrap_UDP::New },
        { "tcp",   Wrap_TCP::New },
        { "sleep", Sleep         },
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
