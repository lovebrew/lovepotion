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

#define CLASS_TYPE  LUAOBJ_TYPE_IMAGE
#define CLASS_NAME  "Image"

const char *imageInit(love_image *self, const char *filename) {

	if (!fileExists(filename)) return "Could not open image. Does not exist.";

	self->texture = sfil_load_PNG_file(filename, SF2D_PLACE_RAM);

	return NULL;

}

int imageNew(lua_State *L) { // love.graphics.newImage()

	const char *filename = luaL_checkstring(L, 1);

	char final[strlen(rootDir) + strlen(filename) + 2];

	combine(final, rootDir, filename);

	love_image *self = luaobj_newudata(L, sizeof(*self));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	const char *error = imageInit(self, final);

	if (error) luaError(L, error);

	return 1;

}

int imageGC(lua_State *L) { // Garbage Collection

	love_image *self = luaobj_checkudata(L, 1, CLASS_TYPE);
	sf2d_free_texture(self->texture);

	return 0;

}

int imageGetDimensions(lua_State *L) { // image:getDimensions()

	love_image *self = luaobj_checkudata(L, 1, CLASS_TYPE);
	lua_pushinteger(L, self->texture->width);
	lua_pushinteger(L, self->texture->height);

	return 2;

}

int imageGetWidth(lua_State *L) { // image:getWidth()

	love_image *self = luaobj_checkudata(L, 1, CLASS_TYPE);
	lua_pushinteger(L, self->texture->width);

	return 1;

}

int imageGetHeight(lua_State *L) { // image:getHeight()

	love_image *self = luaobj_checkudata(L, 1, CLASS_TYPE);
	lua_pushinteger(L, self->texture->height);

	return 1;

}

int initImageClass(lua_State *L) {

	luaL_Reg reg[] = {
		{ "new",            imageNew           },
		{ "__gc",           imageGC            },
		{ "getDimensions",  imageGetDimensions },
		{ "getWidth",       imageGetWidth      },
		{ "getHeight",      imageGetHeight     },
		{ 0, 0 },
	};

  luaobj_newclass(L, CLASS_NAME, NULL, imageNew, reg);

  return 1;

}