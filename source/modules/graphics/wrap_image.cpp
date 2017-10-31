#include "common/runtime.h"
#include "imagedata.h"
#include "image.h"
#include "wrap_image.h"

#define CLASS_TYPE  LUAOBJ_TYPE_IMAGE
#define CLASS_NAME  "Image"

using love::Image;

int imageNew(lua_State * L)
{
	const char * path = luaL_checkstring(L, 1);

	Image * self = (Image *)luaobj_newudata(L, sizeof(* self));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	const char * error = self->Init(path, false);
	
	if (error)
		return luaL_error(L, "Failed to open %s: does not exist.", error);
 
	return 1;
}

int imageGetWidth(lua_State * L)
{
	Image * self = (Image *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushnumber(L, self->GetWidth());

	return 1;
}

int imageGetHeight(lua_State * L)
{
	Image * self = (Image *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushnumber(L, self->GetHeight());
	
	return 1;
}

int imageGetDimensions(lua_State * L)
{
	Image * self = (Image *)luaobj_checkudata(L, 1, CLASS_TYPE);
	
	lua_pushnumber(L, self->GetWidth());
	lua_pushnumber(L, self->GetHeight());
	
	return 2;
}

int imageSetFilter(lua_State * L)
{
	Image * self = (Image *)luaobj_checkudata(L, 1, CLASS_TYPE);

	const char * min = luaL_checkstring(L, 2);
	const char * mag = luaL_checkstring(L, 3);

	self->SetFilter(min, mag);

	return 0;
}

int imageGC(lua_State * L)
{
	Image * self = (Image *)luaobj_checkudata(L, 1, CLASS_TYPE);

	C3D_TexDelete(self->GetTexture());

	return 0;
}

int initImageClass(lua_State *L) {

	luaL_Reg reg[] = {
		{"new",				imageNew		},
		{"getWidth",		imageGetWidth	},
		{"getHeight",		imageGetHeight	},
		{"getDimensions",	imageGetDimensions	},
		{"setFilter",		imageSetFilter	},
		{"__gc",			imageGC			},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, imageNew, reg);

	return 1;

}