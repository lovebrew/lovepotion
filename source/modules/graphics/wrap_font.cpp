#include "common/runtime.h"

#include "imagedata.h"
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

	const char * error = nullptr;
	
	if (path != nullptr)
		error = self->Init(path);
	else
		error = self->DefaultInit();

	if (error)
		console->ThrowError(error);
 
	return 1;
}

int fontGetWidth(lua_State * L)
{
	Font * self = (Font *)luaobj_checkudata(L, 1, CLASS_TYPE);

	const char * text = luaL_checkstring(L, 2);
	int width = 0;

	for (int i = 0; i < strlen(text); i++)
		width += self->GetWidth(text[i]);

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

	//self->Collect();

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