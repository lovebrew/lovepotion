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

int currentScreen = GFX_BOTTOM;

love_font *currentFont;

int transX = 0;
int transY = 0;
bool isPushed = false;

bool is3D = false;

int currentDepth = 0;
float currentLineWidth = 1.0f;

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
			sf2d_draw_line(x, y, x, y + h, currentLineWidth, getCurrentColor());
			sf2d_draw_line(x, y, x + w, y, currentLineWidth, getCurrentColor());

			sf2d_draw_line(x + w, y, x + w, y + h, currentLineWidth, getCurrentColor());
			sf2d_draw_line(x, y + h, x + w, y + h, currentLineWidth, getCurrentColor());
		}

	}

	return 0;

}

static int graphicsCircle(lua_State *L) { // love.graphics.circle()

	if (sf2d_get_current_screen() == currentScreen) {

			//Incoming args
			char *mode = luaL_checkstring(L, 1);
			float x = luaL_checknumber(L, 2);
			float y = luaL_checknumber(L, 3);
			float r = luaL_checknumber(L, 4);

			translateCoords(&x, &y);

			if( strcmp(mode, "line") == 0 ) {

				int pointqty = 16;
				float two_pi = (float)(3.14159265358979323846 * 2);
				if (pointqty <= 0) pointqty = 1;
				float angle_shift = (two_pi / pointqty);
				float phi = .0f;

				int cx, cy = 0; //Curr points
				int px, py = 0; //Prev points
				int fx, fy = 0; //First points

				for (int i = 0; i < pointqty; ++i, phi += angle_shift){

					cx = x + r * cosf(phi);
					cy = y + r * sinf(phi);

					if( i >= 1 ) {
						sf2d_draw_line(cx, cy, px, py, currentLineWidth, getCurrentColor());
					}

					if( i == 0 ) {
						fx = cx;
						fy = cy;
					}

					px = cx;
					py = cy;
				}

				sf2d_draw_line(fx, fy, px, py, currentLineWidth, getCurrentColor());

			}else if (strcmp(mode, "fill") == 0) {

				//Not sure if this workaround is needed anymore?
				//sf2d_draw_line(x, y, x, y, currentLineWidth, RGBA8(0x00, 0x00, 0x00, 0x00)); // Fixes weird circle bug.
				sf2d_draw_fill_circle(x, y, r, getCurrentColor());

			}

	}

	return 0;

}

static int graphicsLine(lua_State *L) { // love.graphics.line()

	if (sf2d_get_current_screen() == currentScreen) {

		int argc = lua_gettop(L);

		//Table version
		if( argc == 1 ) {

			lua_settop(L, 1); //Remove redundant args (This in itself may be redundant)
			luaL_checktype(L, 1, LUA_TTABLE);
			int tableLen = lua_objlen(L, 1);

			if( tableLen >= 4 ) {

				if( tableLen % 2 == 0 ) {

					int x, y, px, py = 0;
					lua_pushnil(L);

					for(int i = 0; i < tableLen; i+=2 )
					{
						px = x;
						py = y;

						lua_rawgeti(L, 1, i+1);
						x = luaL_checknumber(L, -1);
						lua_pop(L, 1);

						lua_rawgeti(L, 1, i+2);
						y = luaL_checknumber(L, -1);
						lua_pop(L, 1);

						if( i >= 2 ) {
							sf2d_draw_line(x, y, px, py, currentLineWidth, getCurrentColor());
						}
					}
					
				}
				else {

					luaL_error(L, "(T)Number of vertex components must be a multiple of two");

				}

			} else {

				luaL_error(L, "(T)Need at least two vertices to draw a line");

			}


		}

		//Argument list version
		else if( argc >= 4 ) {

			if( argc % 2 == 0 ) {

				int x, y, px, py = 0;

				for( int i=0; i < argc; i+=2 )
				{
					px = x;
					py = y;

					x = luaL_checknumber(L, i + 1);
					y = luaL_checknumber(L, i + 2);

					if( i >= 2 )
						sf2d_draw_line(x, y, px, py, currentLineWidth, getCurrentColor());
				}

			} else {

				luaL_error(L, "Number of vertex components must be a multiple of two");

			}

		} else {

			luaL_error(L, "Need at least two vertices to draw a line");

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

		love_image * img = luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGE);
		love_quad * quad = NULL;

		int start = 2;

		if (!lua_isnone(L, 2) && lua_type(L, 2) != LUA_TNUMBER) {
			quad = luaobj_checkudata(L, 2, LUAOBJ_TYPE_QUAD);
			start = 3;
		}
    
		float x = luaL_optnumber(L, start + 0, 0);
		float y = luaL_optnumber(L, start + 1, 0);
		float rotation = luaL_optnumber(L, start + 2, 0);
		float scaleX = luaL_optnumber(L, start + 3, 1);
		float scaleY = luaL_optnumber(L, start + 4, 1);
		float offsetX = luaL_optnumber(L, start + 5, 0);
		float offsetY = luaL_optnumber(L, start + 6, 0);

		float local_sin = sin(rotation);
		float local_cos = cos(rotation);
		x += (local_cos * (-offsetX) - local_sin * (-offsetY) + offsetX);
		y += (local_sin * (-offsetX) + local_cos * (-offsetY) + offsetY);

		x -= offsetX; // This is wrong TODO: Do it right.
		y -= offsetY;
		
		translateCoords(&x, &y);

		if (!quad) {
			sf2d_draw_texture_rotate_scale_hotspot_blend(img->texture, x, y, rotation, scaleX, scaleY, offsetX, offsetY, getCurrentColor());
		} else {
			sf2d_draw_texture_part_rotate_scale_blend(img->texture, x, y, rotation, quad->x, quad->y, quad->width, quad->height, scaleX, scaleY, getCurrentColor());
		}
	}

	return 0;

}

static int graphicsScissor(lua_State *L) { //love.graphics.setScissor()

	if (sf2d_get_current_screen() == currentScreen) {

		GPU_SCISSORMODE mode = GPU_SCISSOR_NORMAL;

		u32 x = luaL_optnumber(L, 1, 0);
		u32 y = luaL_optnumber(L, 2, 0);
		u32 w = luaL_optnumber(L, 3, 0);
		u32 h = luaL_optnumber(L, 4, 0);

		if (!x && !y && !w && !h) {
			mode = GPU_SCISSOR_DISABLE;
		}

		sf2d_set_scissor_test(mode, x, y, w, h);

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
			int x = luaL_optnumber(L, 2, 0);
			int y = luaL_optnumber(L, 3, 0);

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
			int x = luaL_optnumber(L, 2, 0);
			int y = luaL_optnumber(L, 3, 0);
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

	currentLineWidth = luaL_checknumber(L, 1);

	return 0;

}

static int graphicsGetLineWidth(lua_State *L) { // love.graphics.getLineWidth()

	lua_pushnumber(L, currentLineWidth);
	return 1;

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

int canvasRenderStart(love_canvas * canvas);
int canvasRenderEnd();

static int graphicsSetCanvas(lua_State * L) {

	love_canvas * canvas = NULL;

	if (!lua_isnoneornil(L, 1)) {
		canvas = luaobj_checkudata(L, 1, LUAOBJ_TYPE_CANVAS);
		
		canvasRenderStart(canvas);
	} else {
		canvasRenderEnd();
	}
	
	return 0;
}

int imageNew(lua_State *L);
int fontNew(lua_State *L);
int quadNew(lua_State *L);
int canvasNew(lua_State * L);

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
		{ "newCanvas",			canvasNew					},
		{ "setCanvas",			graphicsSetCanvas			},
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
		{ "setLineWidth",		graphicsSetLineWidth		},
		{ "getLineWidth",		graphicsGetLineWidth		},
		{ "setDefaultFilter",	graphicsSetDefaultFilter	},
		{ "getDefaultFilter",	graphicsGetDefaultFilter	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);

	return 1;

}
