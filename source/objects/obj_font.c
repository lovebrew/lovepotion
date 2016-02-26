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

#include "Vera_ttf.h"

int fontLimit = 2;
int fontCounter = 0;

#define CLASS_TYPE  LUAOBJ_TYPE_FONT
#define CLASS_NAME  "Font"

const char *fontInit(love_font *self, const char *filename, int size) {

	if (!fileExists(filename)) return "Could not open font. Does not exist.";

	self->font = sftd_load_font_file(filename);
	self->size = size;

	return NULL;

}

const char *fontDefaultInit(love_font *self, int size) {

	self->font = sftd_load_font_mem(Vera_ttf, Vera_ttf_size);
	self->size = size;

	return NULL;

}

int fontNew(lua_State *L) { // love.graphics.newFont()

	if (fontCounter <= fontLimit) {

		fontCounter += 1;
        
        int argc = lua_gettop(L);
        char *filename = 0;
        int fontSize = 14;
        
        //newFont(fontSize)
        if( argc == 1){
            fontSize = lua_isnoneornil(L, 1) ? NULL : luaL_checkinteger(L, 1);
        }
        
        //newFont(filename, fontsize)
        if( argc == 2 ) {
            filename = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);
            fontSize = lua_isnoneornil(L, 2) ? NULL : luaL_checkinteger(L, 2);
        }

		love_font *self = luaobj_newudata(L, sizeof(*self));

		luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

		if (filename) {

			const char *error = fontInit(self, filename, fontSize);

			if (error) luaError(L, error);

		} else {

			fontDefaultInit(self, fontSize);
		}

		return 1;

	} else {

		luaError(L, "LovePotion currently has a 2 font limit. This limitation will hopefully be lifted in future versions.");
		return 0;

	}


}

int fontGC(lua_State *L) { // Garbage Collection

	fontCounter -= 1;
	love_font *self = luaobj_checkudata(L, 1, CLASS_TYPE);
	sftd_free_font(self->font);

	return 0;

}

int fontGetWidth(lua_State *L) { // font:getWidth()

	love_font *self = luaobj_checkudata(L, 1, CLASS_TYPE);
	const char *text = luaL_checkstring(L, 2);

	lua_pushinteger(L, sftd_get_text_width(self->font, self->size, text));

	return 1;

}

int fontGetHeight(lua_State *L) { // font:getHeight()

	love_font *self = luaobj_checkudata(L, 1, CLASS_TYPE);
	lua_pushinteger(L, self->size);

	return 1;

}

int initFontClass(lua_State *L) {

	luaL_Reg reg[] = {
		{ "new",       fontNew       },
		{ "__gc",      fontGC        },
		{ "getWidth",  fontGetWidth  },
		{ "getHeight", fontGetHeight },
		{ 0, 0 },
	};

  luaobj_newclass(L, CLASS_NAME, NULL, fontNew, reg);

  return 1;

}