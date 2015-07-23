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

int currentR = 0xFF;
int currentG = 0xFF;
int currentB = 0xFF;
int currentA = 0xFF;

int currentScreen = 0;

u32 getCurrentColor() {

	return RGBA8(currentR, currentG, currentB, currentA);

}

static int graphicsBGColor(lua_State *L) { // love.graphics.setBackgroundColor()

	int r = luaL_checkinteger(L, 1);
	int g = luaL_checkinteger(L, 2);
	int b = luaL_checkinteger(L, 3);

	sf2d_set_clear_color(RGBA8(r, g, b, 0xFF));

}

static int graphicsSetColor(lua_State *L) { // love.graphics.setColor()

	int argc = lua_gettop(L);

	int r = luaL_checkinteger(L, 1);
	int g = luaL_checkinteger(L, 2);
	int b = luaL_checkinteger(L, 3);
	int a = NULL;

	if (argc > 3)a = luaL_checkinteger(L, 4);

	if (a == NULL) {
		a = currentA;
	}

	currentR = r;
	currentG = g;
	currentB = b;
	currentA = a;

}

static int graphicsGetColor(lua_State *L) { // love.graphics.getColor()

	lua_pushnumber(L, currentR);
	lua_pushnumber(L, currentG);
	lua_pushnumber(L, currentB);
	lua_pushnumber(L, currentA);

	return 4;

}

static int graphicsRectangle(lua_State *L) { // love.graphics.rectangle()

	const char *mode = luaL_checkstring(L, 1);
	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);
	int w = luaL_checkinteger(L, 4);
	int h = luaL_checkinteger(L, 5);

	if (strcmp(mode, "fill") == 0) {
		sf2d_draw_rectangle(x, y, w, h, getCurrentColor());
	} else if (strcmp(mode, "line") == 0) {
		sf2d_draw_line(x, y, x, y + h, getCurrentColor());
		sf2d_draw_line(x, y, x + w, y, getCurrentColor());

		sf2d_draw_line(x + w, y, x + w, y + h, getCurrentColor());
		sf2d_draw_line(x, y + h, x + w, y + h, getCurrentColor());
	}

	return 0;

}

static int graphicsCircle(lua_State *L) { // love.graphics.circle()

	//const char *mode = luaL_checkstring(L, 1);
	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);
	int r = luaL_checkinteger(L, 4);

	sf2d_draw_line(x, y, x, y, getCurrentColor()); // Fixes weird circle bug.
	sf2d_draw_fill_circle(x, y, r, getCurrentColor());

	return 0;

}

static int graphicsGetScreen(lua_State *L) { // love.graphics.getScreen()

	if (sf2d_get_current_screen() == GFX_TOP) {
		lua_pushstring(L, "top");
	} else if (sf2d_get_current_screen() == GFX_BOTTOM) {
		lua_pushstring(L, "bottom");
	}

	return 1;

}

static int graphicsSetScreen(lua_State *L) { // love.graphics.setScreen()

	const char *screen = luaL_checkstring(L, 1);

	if (screen == "top") {
		currentScreen = 0;
	} else if (screen == "bottom") {
		currentScreen = 1;
	}

	return 0;

}

static int graphicsGetSide(lua_State *L) {

	if (sf2d_get_current_side() == GFX_LEFT) { // love.graphics.getSide()
		lua_pushstring(L, "left");
	} else if (sf2d_get_current_side() == GFX_RIGHT) {
		lua_pushstring(L, "right");
	}

	return 1;

}

int initLoveGraphics(lua_State *L) {

	registerFunction("graphics", "setBackgroundColor", graphicsBGColor);
	registerFunction("graphics", "setColor", graphicsSetColor);
	registerFunction("graphics", "getColor", graphicsGetColor);
	registerFunction("graphics", "rectangle", graphicsRectangle);
	registerFunction("graphics", "circle", graphicsCircle);
	registerFunction("graphics", "getScreen", graphicsGetScreen);
	registerFunction("graphics", "setScreen", graphicsSetScreen);
	registerFunction("graphics", "getSide", graphicsGetSide);

	return 1;

}