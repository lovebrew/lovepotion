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

#include <shared.h>

int currentR = 0xFF;
int currentG = 0xFF;
int currentB = 0xFF;
int currentA = 0xFF;

u32 currentScissorX;
u32 currentScissorY;
u32 currentScissorWidth;
u32 currentScissorHeight;

int currentScreen = GFX_BOTTOM;

love_font *currentFont;

int transX = 0;
int transY = 0;
bool isPushed = false;

bool is3D = false;

int currentDepth = 0;

u32 defaultFilter = GPU_TEXTURE_MAG_FILTER(GPU_LINEAR)|GPU_TEXTURE_MIN_FILTER(GPU_LINEAR); // Default Image Filter.
char *defaultMinFilter = "linear";
char *defaultMagFilter = "linear";

u32 getCurrentColor() {

	return RGBA8(currentR, currentG, currentB, currentA);
 
}

int translateCoords(int *x, int *y) {

	// Emulates the functionality of lg.translate

	if (isPushed) {

		*x += transX;
		*y += transY;

	}

	// Sets depth of objects

	if (is3D && sf2d_get_current_screen() == GFX_TOP) {

		float slider = CONFIG_3D_SLIDERSTATE;

		if (sf2d_get_current_side() == GFX_LEFT) *x -= (slider * currentDepth);
		if (sf2d_get_current_side() == GFX_RIGHT) *x += (slider * currentDepth);

	}

	return 0;

}

static int graphicsSetBackgroundColor(lua_State *L) { // love.graphics.setBackgroundColor()

	int r, g, b;

	if (lua_isnumber(L, -1)) {

		r = luaL_checkinteger(L, 1);
		g = luaL_checkinteger(L, 2);
		b = luaL_checkinteger(L, 3);

	} else if (lua_istable(L, -1)) {

		for (int i = 1; i <= 4; i++) {

			lua_rawgeti(L, 1, i);

		}

		r = luaL_checkinteger(L, -4);
		g = luaL_checkinteger(L, -3);
		b = luaL_checkinteger(L, -2);

	}



	sf2d_set_clear_color(RGBA8(r, g, b, 0xFF));

	return 0;

}

static int graphicsSetColor(lua_State *L) { // love.graphics.setColor()

	if (lua_isnumber(L, -1)) {

		currentR = luaL_checkinteger(L, 1);
		currentG = luaL_checkinteger(L, 2);
		currentB = luaL_checkinteger(L, 3);
		currentA = luaL_optnumber(L, 4, currentA);

	} else if (lua_istable(L, -1)) {

		for (int i = 1; i <= 4; i++) {

			lua_rawgeti(L, 1, i);

		}

		currentR = luaL_checkinteger(L, -4);
		currentG = luaL_checkinteger(L, -3);
		currentB = luaL_checkinteger(L, -2);
		currentA = luaL_optnumber(L, -1, currentA);

	}

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

	if (sf2d_get_current_screen() == currentScreen) {

		char *mode = luaL_checkstring(L, 1);

		int x = luaL_checkinteger(L, 2);
		int y = luaL_checkinteger(L, 3);

		translateCoords(&x, &y);

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

	}

	return 0;

}

static int graphicsCircle(lua_State *L) { // love.graphics.circle()

	if (sf2d_get_current_screen() == currentScreen) {

		int step = 15;

		char *mode = luaL_checkstring(L, 1);
		int x = luaL_checkinteger(L, 2);
		int y = luaL_checkinteger(L, 3);
		int r = luaL_checkinteger(L, 4);

		translateCoords(&x, &y);

		sf2d_draw_line(x, y, x, y, RGBA8(0x00, 0x00, 0x00, 0x00)); // Fixes weird circle bug.
		sf2d_draw_fill_circle(x, y, r, getCurrentColor());

	}

	return 0;

}

static int graphicsLine(lua_State *L) { // love.graphics.line() -- Semi-Broken

	if (sf2d_get_current_screen() == currentScreen) {

		int argc = lua_gettop(L);
		int i = 0;

		if ((argc/2)*2 == argc) {
			for( i; i < argc / 2; i++) {

				int t = i * 4;

				int x1 = luaL_checkinteger(L, t + 1);
				int y1 = luaL_checkinteger(L, t + 2);
				int x2 = luaL_checkinteger(L, t + 3);
				int y2 = luaL_checkinteger(L, t + 4);

				translateCoords(&x1, &y1);
				translateCoords(&x2, &y2);

				sf2d_draw_line(x1, y1, x2, y2, getCurrentColor());

			}
		}

	}

	return 0;

}

static int graphicsGetScreen(lua_State *L) { // love.graphics.getScreen()

	if (currentScreen == GFX_TOP) {
		lua_pushstring(L, "top");
	} else if (currentScreen == GFX_BOTTOM) {
		lua_pushstring(L, "bottom");
	}

	return 1;

}

static int graphicsSetScreen(lua_State *L) { // love.graphics.setScreen()

	char *screen = luaL_checkstring(L, 1);

	if (strcmp(screen, "top") == 0) {
		currentScreen = GFX_TOP;
	} else if (strcmp(screen, "bottom") == 0) {
		currentScreen = GFX_BOTTOM;
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

	if (currentScreen == GFX_TOP) {

		returnWidth = topWidth;

	} else if (currentScreen == GFX_BOTTOM) {

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

	if (sf2d_get_current_screen() == currentScreen) {
		love_image * img = NULL;// = luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGE);
		love_quad * quad = NULL;
		love_spritebatch * spritebatch = NULL;

		int x, y;
		int sx, sy;
		int ox, oy;
		float rad;

		int quadX;
		int quadY;

		int quadWidth;
		int quadHeight;

		int width;
		int height;

 		int index = lua_absindex(L, 1);
  		luaobj_head_t *udata = lua_touserdata(L, index);

		if (udata->type & LUAOBJ_TYPE_IMAGE) {
			img = luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGE);
		} else if (udata->type & LUAOBJ_TYPE_SPRITEBATCH) {
			spritebatch = luaobj_checkudata(L, 1, LUAOBJ_TYPE_SPRITEBATCH);
		}

		if (img) {

			quadX = 0;
			quadY = 0;
			quadWidth = img->texture->width;
			quadHeight = img->texture->height;

			width = quadWidth;
			height = quadHeight;

		} else if (spritebatch) {

			quadX = 0;
			quadY = 0;

			quadWidth = spritebatch->resource->texture->width;
			quadHeight = spritebatch->resource->texture->height;

			width = quadWidth;
			height = quadHeight;
		}

		if (!lua_isnone(L, 2) && lua_type(L, 2) != LUA_TNUMBER) {

			quad = luaobj_checkudata(L, 2, LUAOBJ_TYPE_QUAD);

			if (quad) {
				quadX = quad->x;
				quadY = quad->y;
				quadWidth = quad->width;
				quadHeight = quad->height;
			}

			x = luaL_optnumber(L, 3, 0);
			y = luaL_optnumber(L, 4, 0);
			rad = luaL_optnumber(L, 5, 0);
			sx = luaL_optnumber(L, 6, 1);
			sy = luaL_optnumber(L, 7, 1);
            ox = luaL_optnumber(L, 8, 0);
            oy = luaL_optnumber(L, 9, 0);
            
		} else {

			x = luaL_optnumber(L, 2, 0);
			y = luaL_optnumber(L, 3, 0);
			rad = luaL_optnumber(L, 4, 0);
			sx = luaL_optnumber(L, 5, 1);
			sy = luaL_optnumber(L, 6, 1);
			ox = luaL_optnumber(L, 7, 0);
			oy = luaL_optnumber(L, 8, 0);

		}

		x -= ox;
		y -= oy;

		translateCoords(&x, &y);

		if (!spritebatch && img) {

			sf2d_draw_texture_part_rotate_scale_blend(img->texture, x + img->texture->width / 2, y + img->texture->height / 2, rad, quadX, quadY, quadWidth, quadHeight, sx, sy, getCurrentColor());

		} else {

			for (int i = 0; i < spritebatch->currentImage; i++) {

				sf2d_draw_texture_part_rotate_scale_blend(spritebatch->resource->texture, x + spritebatch->points[i].x + spritebatch->quads[i].width / 2, y + spritebatch->points[i].y + spritebatch->quads[i].height / 2, rad, spritebatch->quads[i].x, spritebatch->quads[i].y, spritebatch->quads[i].width, spritebatch->quads[i].height, sx, sy, getCurrentColor());

			}

		}

	}

	return 0;

}

static int graphicsScissor(lua_State *L) { //love.graphics.setScissor()

	if (sf2d_get_current_screen() == currentScreen) {

		GPU_SCISSORMODE mode = GPU_SCISSOR_NORMAL;

		if (!lua_isnone(L, 1)) {
			currentScissorX = luaL_checkinteger(L, 1);
			currentScissorY = luaL_checkinteger(L, 2);
			currentScissorWidth = luaL_checkinteger(L, 3);
			currentScissorHeight = luaL_checkinteger(L, 4);
		} else {
			mode = GPU_SCISSOR_DISABLE;
		}

		sf2d_set_scissor_test(mode, currentScissorX, currentScissorY, currentScissorWidth, currentScissorHeight);
		
	}

	return 0;

}

static int graphicsSetFont(lua_State *L) { // love.graphics.setFont()

	currentFont = luaobj_checkudata(L, 1, LUAOBJ_TYPE_FONT);

	return 0;

}

static int graphicsPrint(lua_State *L) { // love.graphics.print()

	if (sf2d_get_current_screen() == currentScreen) {

		if (currentFont) {

			char *printText = luaL_checkstring(L, 1);
			int x = luaL_checkinteger(L, 2);
			int y = luaL_checkinteger(L, 3);

			translateCoords(&x, &y);

			sftd_draw_text(currentFont->font, x, y, getCurrentColor(), currentFont->size, printText);

		}

	}

	return 0;

}

static int graphicsPrintFormat(lua_State *L) {

	if (sf2d_get_current_screen() == currentScreen) {

		if (currentFont) {

			char *printText = luaL_checkstring(L, 1);
			int x = luaL_checkinteger(L, 2);
			int y = luaL_checkinteger(L, 3);
			int limit = luaL_checkinteger(L, 4);
			char *align = luaL_optstring(L, 5, "left");

			int width = sftd_get_text_width(currentFont->font, currentFont->size, printText);

			if (strcmp(align, "center") == 0) {

				if (width < limit) {
					x += (limit / 2) - (width / 2);
				}

			} else if (strcmp(align, "right") == 0) {

				if (width < limit) {
					x += limit - width;
				}

			}

			translateCoords(&x, &y);

			if (x > 0) limit += x; // Quick text wrap fix, needs removing once sf2dlib is updated.

			sftd_draw_text_wrap(currentFont->font, x, y, getCurrentColor(), currentFont->size, limit, printText);

		}

	}

	return 0;

}

static int graphicsPush(lua_State *L) { // love.graphics.push()

	if (sf2d_get_current_screen() == currentScreen) {

		isPushed = true;

	}

	return 0;

}

static int graphicsPop(lua_State *L) { // love.graphics.pop()

	if (sf2d_get_current_screen() == currentScreen) {

		transX = 0;
		transY = 0;
		isPushed = false;

	}

	return 0;

}

static int graphicsOrigin(lua_State *L) { // love.graphics.origin()

	if (sf2d_get_current_screen() == currentScreen) {

		transX = 0;
		transY = 0;

	}

	return 0;

}

static int graphicsTranslate(lua_State *L) { // love.graphics.translate()

	if (sf2d_get_current_screen() == currentScreen) {

		int dx = luaL_checkinteger(L, 1);
		int dy = luaL_checkinteger(L, 2);

		transX = transX + dx;
		transY = transY + dy;

	}

	return 0;

}

static int graphicsSet3D(lua_State *L) { // love.graphics.set3D()

	is3D = lua_toboolean(L, 1);
	sf2d_set_3D(is3D);

	return 0;

}

static int graphicsGet3D(lua_State *L) { // love.graphics.get3D()

	lua_pushboolean(L, is3D);

	return 1;

}

static int graphicsSetDepth(lua_State *L) { // love.graphics.setDepth()

	currentDepth = luaL_checkinteger(L, 1);

	return 0;

}

static int graphicsGetDepth(lua_State *L) { // love.graphics.getDepth()

	lua_pushnumber(L, currentDepth);

	return 1;

}

static int graphicsSetLineWidth(lua_State *L) { // love.graphics.setLineWidth()

 // TODO: Do this properly

}

static int graphicsGetLineWidth(lua_State *L) { // love.graphics.getLineWidth()

 // TODO: This too.

}

static int graphicsSetDefaultFilter(lua_State *L) { // love.graphics.setDefaultFilter()

	char *minMode = luaL_checkstring(L, 1);
	char *magMode = luaL_optstring(L, 2, minMode);

	u32 minFilter;
	u32 magFilter;

	if (strcmp(minMode, "linear") != 0 && 
		strcmp(minMode, "nearest") != 0 &&
		strcmp(magMode, "linear") != 0 &&
		strcmp(magMode, "nearest" != 0)) {
			luaError(L, "Invalid Image Filter.");
			return 0;
		}

	if (strcmp(minMode, "linear") == 0) minFilter = GPU_TEXTURE_MIN_FILTER(GPU_LINEAR);
	if (strcmp(magMode, "linear") == 0) magFilter = GPU_TEXTURE_MAG_FILTER(GPU_LINEAR);
	if (strcmp(minMode, "nearest") == 0) minFilter = GPU_TEXTURE_MIN_FILTER(GPU_NEAREST);
	if (strcmp(magMode, "nearest") == 0) magFilter = GPU_TEXTURE_MAG_FILTER(GPU_NEAREST);

	defaultMinFilter = minMode;
	defaultMagFilter = magMode;

	defaultFilter = magFilter | minFilter;

	return 0;

}

static int graphicsGetDefaultFilter(lua_State *L) { // love.graphics.getDefaultFilter()

	lua_pushstring(L, defaultMinFilter);
	lua_pushstring(L, defaultMagFilter);

	return 2;

}

int imageNew(lua_State *L);
int fontNew(lua_State *L);
int quadNew(lua_State *L);
int spriteBatchNew(lua_State *L);

const char *fontDefaultInit(love_font *self, int size);

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
		{ "newFont",			fontNew						},
		{ "newQuad",			quadNew						},
		{ "draw",				graphicsDraw				},
		{ "setFont",			graphicsSetFont				},
		{ "print",				graphicsPrint				},
		{ "printf",				graphicsPrintFormat			},
		{ "push",				graphicsPush				},
		{ "pop",				graphicsPop					},
		{ "origin",				graphicsOrigin				},
		{ "translate",			graphicsTranslate			},
		{ "set3D",				graphicsSet3D				},
		{ "get3D",				graphicsGet3D				},
		{ "setDepth",			graphicsSetDepth			},
		{ "getDepth",			graphicsGetDepth			},
		{ "setScissor",			graphicsScissor				},
		{ "newSpriteBatch",		spriteBatchNew				},
		// { "setLineWidth",		graphicsSetLineWidth		},
		// { "getLineWidth",		graphicsGetLineWidth		},
		{ "setDefaultFilter",	graphicsSetDefaultFilter	},
		{ "getDefaultFilter",	graphicsGetDefaultFilter	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);

	return 1;

}
