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

//Videah: Draw to framebuffer, convert the "image" to a texture and draw it only once

#include "../shared.h"

#define CLASS_TYPE  LUAOBJ_TYPE_SPRITEBATCH
#define CLASS_NAME  "Spritebatch"

const char * spriteBatchInit(love_spritebatch * self, love_image * image, int maxImages) { //love.graphics.newSpriteBatch()

	if (maxImages <= 0) {
		return "Invalid amount of max images: %d";
	}
	
	self->resource = image;

	self->maxImages = maxImages;

	self->currentImage = 0;

	return NULL;

}

int spriteBatchNew(lua_State * L) {

	love_spritebatch *self = luaobj_newudata(L, sizeof(*self));

	love_image *image = luaobj_checkudata(L, LUAOBJ_TYPE_IMAGE, 1);

	int maxImages = luaL_optnumber(L, 2, 1000);

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	const char *error = spriteBatchInit(self, image, maxImages);

	if (error) luaError(L, error);

	return 1;

}

int spriteBatchAdd(lua_State * L) { //Spritebatch:add()

	love_spritebatch * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	if (self->currentImage >= self->maxImages) {
		return 1;
	}

	int x;
	int y;

	love_image * src = self->resource;
	love_quad * quad = NULL;

	if (lua_type(L, 2) != LUA_TNUMBER) {
		quad = luaobj_checkudata(L, 2, LUAOBJ_TYPE_QUAD);

		x = luaL_checkinteger(L, 3);

		y = luaL_checkinteger(L, 4);
	} else {
		x = luaL_checkinteger(L, 2);

		y = luaL_checkinteger(L, 3);
	}

	self->points[self->currentImage].x = x;
	self->points[self->currentImage].y = y;

	self->currentImage++;

	//printf("Adding to batch: #%d {(%d, %d) - %d, %d}\n", self->currentImage, x, y, self->points[self->currentImage].x, self->points[self->currentImage].y);

	lua_pushnumber(L, self->currentImage);
		
	return 1;
}

int initSpriteBatchClass(lua_State *L) {

	luaL_Reg reg[] = {
		{ "new", spriteBatchNew },
		{ "add", spriteBatchAdd },
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, spriteBatchNew, reg);

	return 1;

}
