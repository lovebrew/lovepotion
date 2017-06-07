#include "common/runtime.h"
#include "window.h"

int windowInit(lua_State * L)
{

	luaL_Reg reg[] = 
	{
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}