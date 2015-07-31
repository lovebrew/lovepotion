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
#include <sfil.h>

int currentR = 0xFF;
int currentG = 0xFF;
int currentB = 0xFF;
int currentA = 0xFF;

int TOP_SCREEN = 0;
int BOT_SCREEN = 1;

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

static int graphicsLine(lua_State *L) { // love.graphics.line() -- Semi-Broken

	int argc = lua_gettop(L);
	int i = 0;

	if ((argc/2)*2 == argc) {
		for( i; i < argc / 2; i++) {

			int t = i * 4;

			int x1 = luaL_checkinteger(L, t + 1);
			int y1 = luaL_checkinteger(L, t + 2);
			int x2 = luaL_checkinteger(L, t + 3);
			int y2 = luaL_checkinteger(L, t + 4);

			sf2d_draw_line(x1, y1, x2, y2, getCurrentColor());

		}
	}

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

	if (strcmp(screen, "top") == 0) {
		currentScreen = TOP_SCREEN;
	} else if (strcmp(screen, "bottom") == 0) {
		currentScreen = BOT_SCREEN;
	}

	return 0;

}

static int graphicsGetSide(lua_State *L) { // love.graphics.getSide()

	if (sf2d_get_current_side() == GFX_LEFT) {
		lua_pushstring(L, "left");
	} else if (sf2d_get_current_side() == GFX_RIGHT) {
		lua_pushstring(L, "right");
	}

	return 1;

}

static int graphicsPresent(lua_State *L) { // love.graphics.present()

	sf2d_swapbuffers();

	return 0;

}

static int graphicsGetWidth(lua_State *L) { // love.graphics.getWidth()

	int topWidth = 400;
	int botWidth = 320;

	int returnWidth;

	// TODO: Take screen sides into account.

	if (currentScreen == TOP_SCREEN) {

		returnWidth = topWidth;

	} else if (currentScreen == BOT_SCREEN) {

		returnWidth = botWidth;

	}

	lua_pushnumber(L, returnWidth);

	return 1;

}

static int graphicsGetHeight(lua_State *L) { // love.graphics.getHeight()

	int returnWidth = 240;

	lua_pushnumber(L, returnWidth);

	return 1;

}

static int graphicsNewImage(lua_State *L) {

	const char *path = luaL_checkstring(L, 1);

	sf2d_texture *tex = sfil_load_PNG_file(path, SF2D_PLACE_RAM);

	lua_pushlightuserdata(L, tex);

	return 1;

}

static int graphicsDraw(lua_State *L) {

	sf2d_texture *tex = sfil_load_PNG_buffer(luaL_checkudata(L, 1, ""), SF2D_PLACE_RAM);

	sf2d_draw_texture(tex, 200, 200);

	return 0;

}

int initLoveGraphics(lua_State *L) {

	registerFunction("graphics", "setBackgroundColor", graphicsBGColor);
	registerFunction("graphics", "setColor", graphicsSetColor);
	registerFunction("graphics", "getColor", graphicsGetColor);
	registerFunction("graphics", "rectangle", graphicsRectangle);
	registerFunction("graphics", "circle", graphicsCircle);
	registerFunction("graphics", "line", graphicsLine);
	registerFunction("graphics", "getScreen", graphicsGetScreen);
	registerFunction("graphics", "setScreen", graphicsSetScreen);
	registerFunction("graphics", "getSide", graphicsGetSide);
	registerFunction("graphics", "present", graphicsPresent);
	registerFunction("graphics", "getWidth", graphicsGetWidth);
	registerFunction("graphics", "getHeight", graphicsGetHeight);
	registerFunction("graphics", "newImage", graphicsNewImage);
	registerFunction("graphics", "draw", graphicsDraw);

	return 1;

}