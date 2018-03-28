#include "common/runtime.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "objects/font/glyph.h"
#include "objects/font/font.h"
#include "objects/font/wrap_font.h"

#define CLASS_TYPE LUAOBJ_TYPE_FONT
#define CLASS_NAME "Font"

int fontNew(lua_State * L)
{
	const char * path = NULL;
	int size = 0;

	/*
	** Either first arg is string
	** Or a number
	*/
	if (!lua_isnoneornil(L, 1) && lua_isstring(L, 1))
		path = luaL_checkstring(L, 1);
	else if (!lua_isnoneornil(L, 1) && lua_isnumber(L, 1))
		size = luaL_checknumber(L, 1);
	else if (lua_isnoneornil(L, 1))
		size = 16;

	if (!lua_isnoneornil(L, 2))
		size = luaL_checknumber(L, 2);

	void * raw_self = luaobj_newudata(L, sizeof(Font));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	if (path != NULL) 
	{
		lua_catchexception(L,
			[&]() { Font * self = new (raw_self) Font(path, size); }
		);
	}
	else
	{
		lua_catchexception(L,
			[&]() { Font * self = new (raw_self) Font(size); }
		);
	}

	return 1;
}

/*int fontGetWidth(lua_State * L)
{
	Font * self = (Font *)luaobj_checkudata(L, 1, CLASS_TYPE);

	size_t length;
	const char * text = luaL_checklstring(L, 2, &length);
	int width = 0;

	for (uint i = 0; i < length; i++)
		width += self->GetWidth((int)text[i]);

	lua_pushnumber(L, width);

	return 1;
}

int fontGetHeight(lua_State * L)
{
	Font * self = (Font *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushinteger(L, self->GetHeight());

	return 1;
}*/

int fontGC(lua_State * L)
{
	Font * self = (Font *)luaobj_checkudata(L, 1, CLASS_TYPE);

	self->~Font();

	return 0;
}

int initFontClass(lua_State * L) 
{
	luaL_Reg reg[] = 
	{
		{"new",			fontNew	},
		//{"getWidth",	fontGetWidth},
		//{"getHeight",	fontGetHeight},
		{"__gc",		fontGC	},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, fontNew, reg);

	return 1;
}