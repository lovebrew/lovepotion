#include "common/runtime.h"
#include "image.h"
#include "file.h"
#include "quad.h"
#include "glyph.h"
#include "font.h"
#include "wrap_font.h"

#define CLASS_TYPE LUAOBJ_TYPE_FONT
#define CLASS_NAME "Font"

using love::Font;

int fontNew(lua_State * L)
{
	const char * path = nullptr;
	
	if (!lua_isnoneornil(L, 1))
		path = luaL_checkstring(L, 1);

	Font * self = (Font *)luaobj_newudata(L, sizeof(* self));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	char * error = nullptr;
	
	if (path != nullptr)
		error = self->Init(path);
	else
		error = self->DefaultInit();

	if (error)
		console->ThrowError(error);
 
	return 1;
}

int fontGC(lua_State * L)
{
	return 0;
}

int initFontClass(lua_State * L) {

	luaL_Reg reg[] = 
	{
		{"new",			fontNew	},
		{"__gc",		fontGC	},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, fontNew, reg);

	return 1;

}