#include "common/runtime.h"
#include "font.h"
#include "wrap_font.h"

#define CLASS_TYPE LUAOBJ_TYPE_FONT
#define CLASS_NAME "Font"

using love::Font;

int fontNew(lua_State * L)
{
	const char * path = luaL_checkstring(L, 1);

	Font * self = (Font *)luaobj_newudata(L, sizeof(* self));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	char * error = self->Init(path);
	
	if (error)
		luaL_error(L, error);
 
	return 1;
}

int initFontClass(lua_State * L) {

	luaL_Reg reg[] = {
		{"new",			fontNew	},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, fontNew, reg);

	return 1;

}