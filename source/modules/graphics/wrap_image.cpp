#include "common/runtime.h"
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

	char * error = self->Init(path);
	
	if (error)
		luaL_error(L, error);
 
	return 1;
}

int imageGC(lua_State * L)
{
	return 0;
}

int initImageClass(lua_State *L) {

	luaL_Reg reg[] = {
		{"new",			imageNew	},
		{"__gc",		imageGC		},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, imageNew, reg);

	return 1;

}