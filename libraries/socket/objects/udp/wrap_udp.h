#pragma once

#include "objects/socket/socket.h"
#include "objects/udp/udp.h"

namespace Wrap_UDP
{
    int Create(lua_State * L);

    int Register(lua_State * L);
}
