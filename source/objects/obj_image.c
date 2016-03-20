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

#define CLASS_TYPE  LUAOBJ_TYPE_IMAGE
#define CLASS_NAME  "Image"

// NoGame Graphics
#include "mosaic_png.h"
#include "toast_back_png.h"
#include "toast_front_png.h"
#include "toast_eyes_closed_png.h"
#include "toast_eyes_open_png.h"
#include "toast_mouth_png.h"

// This big wall of code is a result of not knowing a better way to load nogame graphics.
// If you wanted to use the nogame graphics in your game
// then here's a way to do it I guess.

bool loadNoGameGraphics(love_image *self, const char *filename) {

	if (strncmp(filename, "nogame:mosaic", 13) == 0) {
		self->texture = sfil_load_PNG_buffer(mosaic_png, SF2D_PLACE_RAM);
	} else if (strncmp(filename, "nogame:toast_back", 17) == 0) {
		self->texture = sfil_load_PNG_buffer(toast_back_png, SF2D_PLACE_RAM);
	} else if (strncmp(filename, "nogame:toast_front", 18) == 0) {
		self->texture = sfil_load_PNG_buffer(toast_front_png, SF2D_PLACE_RAM);
	} else if (strncmp(filename, "nogame:toast_eyes_closed", 24) == 0) {
		self->texture = sfil_load_PNG_buffer(toast_eyes_closed_png, SF2D_PLACE_RAM);
	} else if (strncmp(filename, "nogame:toast_eyes_open", 22) == 0) {
		self->texture = sfil_load_PNG_buffer(toast_eyes_open_png, SF2D_PLACE_RAM);
	} else if (strncmp(filename, "nogame:toast_mouth", 18) == 0) {
		self->texture = sfil_load_PNG_buffer(toast_mouth_png, SF2D_PLACE_RAM);
	} else {
		return false;
	}

	return true;

}

const char *imageInit(love_image *self, const char *filename) {

	int type = getType(filename);

	if (!loadNoGameGraphics(self, filename)) {

		if (!fileExists(filename)) {
			luaError(L, "Could not open image, does not exist");
			return NULL;
		}

		if (type == 0) { // PNG

			self->texture = sfil_load_PNG_file(filename, SF2D_PLACE_RAM);

		} else if (type == 1) { // JPG
			
			self->texture = sfil_load_JPEG_file(filename, SF2D_PLACE_RAM);

		} else if (type == 2) { // BMP
			
			self->texture = sfil_load_BMP_file(filename, SF2D_PLACE_RAM);

		} else if (type == 4) {

			luaError(L, "Unknown image type");

		}

	}

	return NULL;

}

int imageNew(lua_State *L) { // love.graphics.newImage()

	const char *filename = luaL_checkstring(L, 1);

	love_image *self = luaobj_newudata(L, sizeof(*self));
	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	const char *error = imageInit(self, filename);
	if (error) luaError(L, error);

	sf2d_texture_set_params(self->texture, defaultFilter);
	self->minFilter = defaultMinFilter;
	self->magFilter = defaultMagFilter;

	return 1;

}

int imageGC(lua_State *L) { // Garbage Collection

	love_image *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	if (!self->texture) return 0;

	sf2d_free_texture(self->texture);
	self->texture = NULL;

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

int imageSetFilter(lua_State *L) { // image:setFilter()

	love_image *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	char *minMode = luaL_checkstring(L, 2);
	char *magMode = luaL_optstring(L, 3, minMode);

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

	sf2d_texture_set_params(self->texture, magFilter | minFilter);

	self->minFilter = minMode;
	self->magFilter = magMode;

	return 0;

}

int imageGetFilter(lua_State *L) { // image:getFilter()

	love_image *self = luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushstring(L, self->minFilter);
	lua_pushstring(L, self->magFilter);

	return 2;

}

int initImageClass(lua_State *L) {

	luaL_Reg reg[] = {
		{ "new",            imageNew           },
		{ "__gc",           imageGC            },
		{ "getDimensions",  imageGetDimensions },
		{ "getWidth",       imageGetWidth      },
		{ "getHeight",      imageGetHeight     },
		{ "setFilter",      imageSetFilter     },
		{ "getFilter",      imageGetFilter     },
		{ 0, 0 },
	};

  luaobj_newclass(L, CLASS_NAME, NULL, imageNew, reg);

  return 1;

}
