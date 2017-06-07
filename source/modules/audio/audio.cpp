#include "common/runtime.h"
#include "audio.h"
#include "wrap_source.h"
#include "source.h"

int audioInit(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "newSource",	sourceNew	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}