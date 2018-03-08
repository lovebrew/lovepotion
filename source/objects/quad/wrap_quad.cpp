#include "runtime.h"
#include "quad.h"
#include "wrap_quad.h"

#define CLASS_NAME "Quad"
#define CLASS_TYPE LUAOBJ_TYPE_QUAD

int quadNew(lua_State * L)
{
	int x = luaL_checknumber(L, 1);
	int y = luaL_checknumber(L, 2);

	int width = luaL_checknumber(L, 3);
	int height = luaL_checknumber(L, 4);

	int atlasWidth = luaL_checknumber(L, 5);
	int atlasHeight = luaL_checknumber(L, 6);

	void * raw_self = luaobj_newudata(L, sizeof(Quad));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	Quad * self = new (raw_self) Quad(x, y, width, height, atlasWidth, atlasHeight);
 
	return 1;
}

int quadGC(lua_State * L)
{
	Quad * self = (Quad *)luaobj_checkudata(L, 1, CLASS_TYPE);

	self->~Quad();

	return 0;
}

int initQuadClass(lua_State * L)
{
	luaL_Reg reg[] = {
		{"new",			quadNew		},
		{"__gc",		quadGC		},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, quadNew, reg);

	return 1;
}