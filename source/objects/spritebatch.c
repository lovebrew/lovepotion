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

	self->elements[maxImages];

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

	if (self->currentImage < self->maxImages) {
		int x = luaL_checkinteger(L, 2);

		int y = luaL_checkinteger(L, 3);

		sf2d_vertex_pos_tex *vertices = sf2d_pool_memalign(4 * sizeof(sf2d_vertex_pos_tex), 8);
		if (!vertices) return;

		int w = self->resource->texture->width;
		int h = self->resource->texture->height;

		vertices[0].position = (sf2d_vector_3f){(float)x,   (float)y,   SF2D_DEFAULT_DEPTH};
		vertices[1].position = (sf2d_vector_3f){(float)x+w, (float)y,   SF2D_DEFAULT_DEPTH};
		vertices[2].position = (sf2d_vector_3f){(float)x,   (float)y+h, SF2D_DEFAULT_DEPTH};
		vertices[3].position = (sf2d_vector_3f){(float)x+w, (float)y+h, SF2D_DEFAULT_DEPTH};

		float u = self->resource->texture->width/(float)self->resource->texture->pow2_w;
		float v = self->resource->texture->height/(float)self->resource->texture->pow2_h;

		vertices[0].texcoord = (sf2d_vector_2f){0.0f, 0.0f};
		vertices[1].texcoord = (sf2d_vector_2f){u,    0.0f};
		vertices[2].texcoord = (sf2d_vector_2f){0.0f, v};
		vertices[3].texcoord = (sf2d_vector_2f){u,    v};

		self->elements[self->currentImage] = vertices;

		self->currentImage++;

		printf("Added resource to Spritebatch..\n");
	}

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
