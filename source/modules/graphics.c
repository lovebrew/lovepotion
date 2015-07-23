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

u32* currentColor;

static int graphicsRectangle(lua_State *L) {

	const char *mode = luaL_checkstring(L, 1);
	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);
	int w = luaL_checkinteger(L, 4);
	int h = luaL_checkinteger(L, 5);

	if (strcmp(mode, "fill") == 0) {
		sf2d_draw_rectangle(x, y, w, h, RGBA8(0xFF, 0x00, 0x00, 0xFF));
	} else if (strcmp(mode, "line") == 0) {
		sf2d_draw_line(x, y, x, y + h, RGBA8(0xFF, 0x00, 0x00, 0xFF));
		sf2d_draw_line(x, y, x + w, y, RGBA8(0xFF, 0x00, 0x00, 0xFF));

		sf2d_draw_line(x + w, y, x + w, y + h, RGBA8(0xFF, 0x00, 0x00, 0xFF));
		sf2d_draw_line(x, y + h, x + w, y + h, RGBA8(0xFF, 0x00, 0x00, 0xFF));
	}

	return 0;

}

static int graphicsCircle(lua_State *L) {

	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);
	int r = luaL_checkinteger(L, 3);

	sf2d_draw_line(x, y, x, y, RGBA8(0xFF, 0x00, 0x00, 0xFF)); // Fixes weird circle bug.
	sf2d_draw_fill_circle(x, y, r, RGBA8(0xFF, 0x00, 0x00, 0xFF));

	return 0;

}

int initLoveGraphics(lua_State *L) {
	registerFunction("graphics", "rectangle", graphicsRectangle);
	registerFunction("graphics", "circle", graphicsCircle);
	return 1;
}