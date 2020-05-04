#pragma once

#include "common/runtime.h"

#include "socket/objects/http/wrap_http.h"
#include "socket/objects/socket/wrap_socket.h"
#include "socket/objects/udp/wrap_udp.h"

#if defined (_3DS)
    #define SO_KEEPALIVE 0x0008
    #define SO_DONTROUTE 0x0010
#elif defined (__SWITCH__)

#endif

#define SO_MAX_BUFSIZE 0x100000

namespace LuaSocket
{
    int Open(lua_State * L);

    void NewClass(lua_State * L, const char * classname, ...);

    int ToString(lua_State * L);

    int OpenHTTP(lua_State * L);
}
