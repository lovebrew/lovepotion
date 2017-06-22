#include "common/runtime.h"
#include "quad.h"
#include "wrap_quad.h"

#define CLASS_NAME "Quad"
#define CLASS_TYPE LUAOBJ_TYPE_QUAD

using love::Quad;

int quadNew(lua_State * L)
{
	int x = luaL_checknumber(L, 1);
	int y = luaL_checknumber(L, 2);

	int width = luaL_checknumber(L, 3);
	int height = luaL_checknumber(L, 4);

	Quad * self = (Quad *)luaobj_newudata(L, sizeof(* self));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	char * error = self->Init(x, y, width, height);
	
	if (error)
		console->ThrowError(error);
 
	return 1;
}

int quadGC(lua_State * L)
{
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