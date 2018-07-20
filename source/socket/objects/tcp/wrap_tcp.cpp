#include "common/runtime.h"
#include "socket/common.h"

#include "socket/objects/socket.h"

#include "socket/objects/tcp/udp.h"
#include "socket/objects/tcp/wrap_udp.h"

#define CLASS_NAME "TCP"
#define CLASS_TYPE LUAOBJ_TYPE_TCP

int tcpNew(lua_State * L)
{
    void * raw_self = luaobj_newudata(L, sizeof(TCP));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    TCP * self = new (raw_self) TCP();

    return 1;
}

int tcpBind(lua_State * L)
{
	string ip = luaL_checkstring(L, 1);
	int port = luaL_checkinteger(L, 2);

	int ret = tcpNew(L);

	TCP * self = (TCP *)luaobj_checkudata(L, -1, LUAOBJ_TYPE_TCP);

	self->Bind(ip, port);

	return ret;
}

int tcpGC(lua_State * L)
{
	return 0;
}

int tcpToString(lua_State * L)
{
	TCP * self = (TCP *)luaobj_checkudata(L, 1, CLASS_TYPE);

    char * data = self->ToString();

    lua_pushstring(L, data);

    free(data);

    return 1;
}

int initTcpClass(lua_State * L)
{
	luaL_Reg reg[] = 
    {
        { "__gc",       tcpGC          },
        { "__tostring", tcpToString    },
        { "new",		tcpNew         },
        { 0, 0 },
    };

    luaobj_newclass(L, CLASS_NAME, NULL, tcpNew, reg);

    return 1;
}