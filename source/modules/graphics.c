#include "../include/lua/lua.h"
#include "../include/lua/lualib.h"
#include "../include/lua/lauxlib.h"

#include "../shared.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <3ds.h>
#include <sf2d.h>

static int graphicsRectangle(lua_State *L) {

	int argc = lua_gettop(L);
	int x = luaL_checkinteger(L,1);
	int y = luaL_checkinteger(L,2);
	int w = luaL_checkinteger(L,3);
	int h = luaL_checkinteger(L,4);

	sf2d_draw_rectangle(x, y, w, h, RGBA8(0xFF, 0x00, 0x00, 0xFF));

	return 0;

}

int initLoveGraphics(lua_State *L) {
	registerFunction("graphics", "rectangle", graphicsRectangle);
	return 1;
}