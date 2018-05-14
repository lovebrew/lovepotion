#include "runtime.h"

#include "quad.h"

#include "glyph.h"
#include "font.h"
#include "wrap_font.h"

#define CLASS_TYPE LUAOBJ_TYPE_FONT
#define CLASS_NAME "Font"

int fontNew(lua_State * L)
{
	const char * path = NULL;
	
	if (!lua_isnoneornil(L, 1))
		path = luaL_checkstring(L, 1);

	void * raw_self = luaobj_newudata(L, sizeof(Font));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	Font * self = new (raw_self) Font(path);

	return 1;
}

int fontGetWidth(lua_State * L)
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
}

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
		{"getWidth",	fontGetWidth},
		{"getHeight",	fontGetHeight},
		{"__gc",		fontGC	},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, fontNew, reg);

	return 1;
}