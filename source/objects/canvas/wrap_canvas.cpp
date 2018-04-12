#include "common/runtime.h"

#include "common/drawable.h"
#include "objects/canvas/canvas.h"
#include "objects/canvas/wrap_canvas.h"

#define CLASS_TYPE LUAOBJ_TYPE_CANVAS
#define CLASS_NAME "Canvas"

int canvasNew(lua_State * L)
{
	int width = luaL_optnumber(L, 1, 1280);
	int height = luaL_optnumber(L, 2, 720);

	void * raw_self = (void *)luaobj_newudata(L, sizeof(Canvas));
	Canvas * self = new (raw_self) Canvas(width, height);

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	return 1;
}

int canvasGC(lua_State * L)
{
	Canvas * self = (Canvas *)luaobj_checkudata(L, 1, CLASS_TYPE);

	self->~Drawable();

	return 0;
}

int initCanvasClass(lua_State * L) 
{
	luaL_Reg reg[] = 
	{
		{"new",			canvasNew	},
		{"__gc",		canvasGC	},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, canvasNew, reg);

	return 1;
}