#pragma once

#include "common/runtime.h"

#include "socket/objects/http/wrap_http.h"
#include "socket/objects/socket/wrap_socket.h"
#include "socket/objects/udp/wrap_udp.h"
#include "socket/objects/tcp/wrap_tcp.h"

#define SO_MAX_BUFSIZE 0x100000

namespace LuaSocket
{
    int Open(lua_State * L);

    void NewClass(lua_State * L, const char * classname, ...);

    int ToString(lua_State * L);

    /*
    ** Note: The function socket.bind is available
    ** It is a shortcut for the creation of server TCP sockets.
    */
    int Bind(lua_State * L);

    int Sleep(lua_State * L);

    int OpenHTTP(lua_State * L);
}
