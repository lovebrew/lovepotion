#include "common/runtime.h"
#include "http.h"

extern int udpInit(lua_State * L);

struct { char *name; int (*fn)(lua_State *L); void (*close)(void); } modules[] = 
{
	{"udp", udpInit, NULL },
	{0},
};

void httpClose()
{
	socExit();
}

int httpInit(lua_State * L)
{
	u32 * socketBuffer = (u32 *)memalign(0x1000, 0x100000);

	Result socketInit = socInit(socketBuffer, 0x100000);

	if (R_FAILED(socketInit))
		console->ThrowError("Failed to initialize LuaSocket.");

	luaL_Reg reg[] = 
	{
		{ 0, 0 }
	};

	luaL_newlib(L, reg);

	for (int i = 0; modules[i].name; i++)
	{
		modules[i].fn(L);
		lua_setfield(L, -2, modules[i].name);
	}

	printf("Loaded sockets!\n");
	
	return 1;
}