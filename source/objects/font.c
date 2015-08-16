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

#define CLASS_TYPE  LUAOBJ_TYPE_FONT
#define CLASS_NAME  "Font"

const char *fontInit(love_font *self, const char *filename, int size) {

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

	const char *filename = lua_isnoneornil(L, 1) ? NULL : luaL_checkstring(L, 1);

	love_font *self = luaobj_newudata(L, sizeof(*self));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	if (filename) {

		const char *error = fontInit(self, filename, 16);

	} else {

		fontDefaultInit(self, 16);
	}

	return 1;

}

int fontGC(lua_State *L) { // Garbage Collection

	love_font *self = luaobj_checkudata(L, 1, CLASS_TYPE);
	sftd_free_font(self);

	return 0;

}

int initFontClass(lua_State *L) {

	luaL_Reg reg[] = {
		{ "new",   fontNew           },
		{ "__gc",  fontGC            },
		{ 0, 0 },
	};

  luaobj_newclass(L, CLASS_NAME, NULL, fontNew, reg);

  return 1;

}