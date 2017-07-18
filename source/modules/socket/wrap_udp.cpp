#include "common/runtime.h"
#include "wrap_udp.h"
#include "udp.h"

#define CLASS_NAME "UDP"
#define CLASS_TYPE LUAOBJ_TYPE_UDP

int udpNew(lua_State * L)
{
	printf("New UDP socket!\n");

	return 1;
}

int udpInit(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{0, 0}
	};

	luaobj_newclass(L, CLASS_NAME, NULL, udpNew, reg);

	return 1;
}