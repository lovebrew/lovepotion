#include "common/runtime.h"
#include "socket.h"
#include "wrap_udp.h"

void socketClose()
{
	socExit();
}

void initLuaSocket(lua_State * L)
{
	//Preload our package in package.preload
	lua_getglobal(L, "package"); 
	lua_getfield(L, -1, "preload");

	//push the C function for init here
	lua_pushcfunction(L, socketInit);
	
	//Set field name
	lua_setfield(L, -2, "socket");

	//throw onto the stack
	lua_pop(L, 2);
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