#pragma once

#include "common/runtime.h"

#include "socket/objects/http/wrap_http.h"
#include "socket/objects/socket/wrap_socket.h"
#include "socket/objects/udp/wrap_udp.h"

#define SO_MAX_BUFSIZE 0x100000

namespace LuaSocket
{
    int Open(lua_State * L);

    void NewClass(lua_State * L, const char * classname, ...);

    int ToString(lua_State * L);

    int OpenHTTP(lua_State * L);
}
