#include "common/runtime.h"
#include "socket/common.h"

#include "socket/socket.h"
#include "socket/objects/udp/wrap_udp.h"

int Socket::Initialize(lua_State * L)
{
	Result ret = socketInitializeDefault();

	if (ret != 0)
		Console::ThrowError("Failed to load LuaSocket!");

	int (*classes[])(lua_State *L) = 
	{
		initUDPClass,
		NULL,
	};

	for (int i = 0; classes[i]; i++) 
	{
		classes[i](L);
		lua_pop(L, 1);
	}

	luaL_Reg reg[] = 
	{
		{"udp", udpNew},
		{0, 0},
	};

	luaL_newlib(L, reg);

	return 1;
}

void Socket::Close()
{
	socketExit();
}