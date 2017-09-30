#include "common/runtime.h"
#include "canvas.h"

#define CLASS_TYPE LUAOBJ_TYPE_CANVAS
#define CLASS_NAME "Canvas"

using love::Canvas;

int canvasNew(lua_State * L)
{
	int width = luaL_optnumber(L, 1, 400);
	int height = luaL_optnumber(L, 2, 240);

	Canvas * self = (Canvas *)luaobj_newudata(L, sizeof(* self));
	self->Init(width, height);

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	return 1;
}

int canvasGC(lua_State * L)
{
	Canvas * self = (Canvas *)luaobj_checkudata(L, 1, CLASS_TYPE);

	//self->Collect();

	return 0;
}

int initCanvasClass(lua_State * L) 
{
	luaL_Reg reg[] = 
	{
		{"new",			canvasNew	},
		//{"getWidth",	fontGetWidth},
		//{"getHeight",	fontGetHeight},
		{"__gc",		canvasGC	},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, canvasNew, reg);

	return 1;

}