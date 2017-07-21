#include "common/runtime.h"
#include "socket.h"
#include "wrap_udp.h"

void socketClose()
{
	socExit();
}

int socketInit(lua_State * L)
{
	u32 * socketBuffer = (u32 *)memalign(0x1000, 0x100000);

	Result socketsInit = socInit(socketBuffer, 0x100000);

	if (R_FAILED(socketsInit))
		console->ThrowError("Failed to initialize LuaSocket.");
	
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