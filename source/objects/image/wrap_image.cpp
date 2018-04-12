#include "common/runtime.h"

#include "common/drawable.h"
#include "objects/image/image.h"
#include "objects/image/wrap_image.h"

#define CLASS_TYPE  LUAOBJ_TYPE_IMAGE
#define CLASS_NAME  "Image"

int imageNew(lua_State * L)
{
	const char * path = luaL_checkstring(L, 1);

	void * raw_self = luaobj_newudata(L, sizeof(Image));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	bool memory = false;

	lua_getglobal(L, "in_error");
	if (lua_toboolean(L, -1) == 1)
		memory = true;
	
	lua_setglobal(L, "in_error");

	Image * self = new (raw_self) Image(path, memory);

	return 1;
}

//Image:getWidth
int imageGetWidth(lua_State * L)
{
	Image * self = (Image *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushnumber(L, self->GetWidth());

	return 1;
}

//Image:getHeight
int imageGetHeight(lua_State * L)
{
	Image * self = (Image *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushnumber(L, self->GetHeight());

	return 1;
}

//Image:getDimensions
int imageGetDimensions(lua_State * L)
{
	Image * self = (Image *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushnumber(L, self->GetWidth());
	lua_pushnumber(L, self->GetHeight());

	return 2;
}

int imageGC(lua_State * L)
{
	Image * self = (Image *)luaobj_checkudata(L, 1, CLASS_TYPE);

	self->~Drawable();

	return 0;
}

int initImageClass(lua_State * L) 
{
	luaL_Reg reg[] = {
		{"new",				imageNew			},
		{"getWidth",		imageGetWidth		},
		{"getHeight",		imageGetHeight		},
		{"getDimensions",	imageGetDimensions	},
		//{"setFilter",		imageSetFilter		},
		{"__gc",			imageGC				},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, imageNew, reg);

	return 1;
}