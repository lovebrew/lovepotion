#include "common/runtime.h"
#include "imagedata.h"
#include "m_image.h"

int imageInit(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "newImageData", imageDataNew	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}