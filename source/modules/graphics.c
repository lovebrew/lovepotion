// This code is licensed under the MIT Open Source License.

// Copyright (c) 2015 Ruairidh Carmichael - ruairidhcarmichael@live.co.uk

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "../shared.h"

#include "Vera_ttf.h"

int currentR = 0xFF;
int currentG = 0xFF;
int currentB = 0xFF;
int currentA = 0xFF;

int TOP_SCREEN = 0;
int BOT_SCREEN = 1;

int currentScreen = 0;

sftd_font *currentFont;
int currentFontSize = 16;

u32 getCurrentColor() {

	return RGBA8(currentR, currentG, currentB, currentA);
 
}

static int graphicsSetBackgroundColor(lua_State *L) { // love.graphics.setBackgroundColor()

	int r = luaL_checkinteger(L, 1);
	int g = luaL_checkinteger(L, 2);
	int b = luaL_checkinteger(L, 3);

	sf2d_set_clear_color(RGBA8(r, g, b, 0xFF));

	return 0;

}

static int graphicsSetColor(lua_State *L) { // love.graphics.setColor()

	int argc = lua_gettop(L);

	int r = luaL_checkinteger(L, 1);
	int g = luaL_checkinteger(L, 2);
	int b = luaL_checkinteger(L, 3);
	int a = NULL;

	if (argc > 3) {
		a = luaL_checkinteger(L, 4);
	}

	if (a == NULL) {
		a = currentA;
	}

	currentR = r;
	currentG = g;
	currentB = b;
	currentA = a;

	return 0;

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

static int graphicsDraw(lua_State *L) { // love.graphics.draw()

	sf2d_texture *img = luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGE);

	int x = luaL_checkinteger(L, 2);	
	int y = luaL_checkinteger(L, 3);	

	sf2d_draw_texture(img, x, y);

	return 0;

}

static int graphicsSetFont(lua_State *L) { // love.graphics.setFont() -- Old, needs updating to 0.8.0+

	int argc = lua_gettop(L);

	if (argc == 0) {

		currentFont = sftd_load_font_mem(Vera_ttf, Vera_ttf_size);
		currentFontSize = 12;

	} else {

		const char *path = luaL_checkstring(L, 1);
		currentFontSize = luaL_checkinteger(L, 2);

		currentFont = sftd_load_font_file(path);

	}

	return 0;

}

static int graphicsPrint(lua_State *L) { // love.graphics.print() -- Partial

	const char *printText = luaL_checkstring(L, 1);
	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);

	sftd_draw_text(currentFont, x / 2, (y + currentFontSize) / 2, getCurrentColor(), currentFontSize, printText);

	return 0;

}

int imageNew(lua_State *L);

int initLoveGraphics(lua_State *L) {

	luaL_Reg reg[] = {
		{ "setBackgroundColor",	graphicsSetBackgroundColor	},
		{ "setColor",			graphicsSetColor			},
		{ "getColor",			graphicsGetColor			},
		{ "rectangle",			graphicsRectangle			},
		{ "circle",				graphicsCircle				},
		{ "line",				graphicsLine				},
		{ "getScreen",			graphicsGetScreen			},
		{ "setScreen",			graphicsSetScreen			},
		{ "getSide",			graphicsGetSide				},
		{ "present",			graphicsPresent				},
		{ "getWidth",			graphicsGetWidth			},
		{ "getHeight",			graphicsGetHeight			},
		{ "newImage",			imageNew					},
		{ "draw",				graphicsDraw				},
		{ "setFont",			graphicsSetFont				},
		{ "print",				graphicsPrint				},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);

	return 1;

}