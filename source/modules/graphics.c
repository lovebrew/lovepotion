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

static int graphicsBGColor(lua_State *L) { // love.graphics.setBackgroundColor()

	int r = luaL_checkinteger(L, 1);
	int g = luaL_checkinteger(L, 2);
	int b = luaL_checkinteger(L, 3);

	sf2d_set_clear_color(RGBA8(r, g, b, 0xFF));

}

static int graphicsSetColor(lua_State *L) { // love.graphics.setColor()

	int r = luaL_checkinteger(L, 1);
	int g = luaL_checkinteger(L, 2);
	int b = luaL_checkinteger(L, 3);
	int a = luaL_checkinteger(L, 4);

	currentColor = RGBA8(r, g, b, a);

}

static int graphicsRectangle(lua_State *L) { // love.graphics.rectangle()

	const char *mode = luaL_checkstring(L, 1);
	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);
	int w = luaL_checkinteger(L, 4);
	int h = luaL_checkinteger(L, 5);

	if (strcmp(mode, "fill") == 0) {
		sf2d_draw_rectangle(x, y, w, h, currentColor);
	} else if (strcmp(mode, "line") == 0) {
		sf2d_draw_line(x, y, x, y + h, currentColor);
		sf2d_draw_line(x, y, x + w, y, currentColor);

		sf2d_draw_line(x + w, y, x + w, y + h, currentColor);
		sf2d_draw_line(x, y + h, x + w, y + h, currentColor);
	}

	return 0;

}

static int graphicsCircle(lua_State *L) { // love.graphics.circle()

	//const char *mode = luaL_checkstring(L, 1);
	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);
	int r = luaL_checkinteger(L, 4);

	sf2d_draw_line(x, y, x, y, currentColor); // Fixes weird circle bug.
	sf2d_draw_fill_circle(x, y, r, currentColor);

	return 0;

}

int initLoveGraphics(lua_State *L) {

	registerFunction("graphics", "setBackgroundColor", graphicsBGColor);
	registerFunction("graphics", "setColor", graphicsSetColor);
	registerFunction("graphics", "rectangle", graphicsRectangle);
	registerFunction("graphics", "circle", graphicsCircle);

	return 1;

}