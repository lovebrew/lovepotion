#include "common/runtime.h"
#include "imagedata.h"
#include "wrap_imagedata.h"

#define CLASS_NAME "ImageData"
#define CLASS_TYPE LUAOBJ_TYPE_IMAGEDATA

using love::ImageData;

int imageDataNew(lua_State * L)
{
	const char * path = luaL_checkstring(L, 1);
	
	ImageData * self = (ImageData *)luaobj_newudata(L, sizeof(* self));
	
	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);
	
	const char * error = self->Init(path);
		
	if (error)
		console->ThrowError(error);

	return 1;
}

int imageDataGetWidth(lua_State * L)
{
	ImageData * self = (ImageData *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushnumber(L, self->GetWidth());

	return 1;
}

int imageDataGetHeight(lua_State * L)
{
	ImageData * self = (ImageData *)luaobj_checkudata(L, 1, CLASS_TYPE);
	
	lua_pushnumber(L, self->GetHeight());
	
	return 1;
}

int imageDataSetPixel(lua_State * L)
{
	ImageData * self = (ImageData *)luaobj_checkudata(L, 1, CLASS_TYPE);

	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);

	int r = luaL_checkinteger(L, 4);
	int g = luaL_checkinteger(L, 5);
	int b = luaL_checkinteger(L, 6);
	int a = luaL_checkinteger(L, 7);
	
	u32 color = ( ( (a & 0xFF) << 24 ) | ( (b & 0xFF) << 16) | ( (g & 0xFF) << 8) | ( (r & 0xFF) << 0 ) );

	self->SetPixel(x, y, color);

	return 0;
}

int imageDataGC(lua_State * L)
{
	return 0;
}

int initImageDataClass(lua_State *L) {
	
		luaL_Reg reg[] = {
			{"new",			imageDataNew		},
			{"getWidth",	imageDataGetWidth	},
			{"getHeight",	imageDataGetHeight	},
			{"__gc",		imageDataGC			},
			{ 0, 0 },
		};
	
		luaobj_newclass(L, CLASS_NAME, NULL, imageDataNew, reg);
	
		return 1;
	
	}