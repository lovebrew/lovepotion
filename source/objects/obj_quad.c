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

#define CLASS_TYPE  LUAOBJ_TYPE_QUAD
#define CLASS_NAME  "Quad"

const char *quadInit(love_quad *self, int x, int y, int width, int height) {

	self->x = luaL_checkint(L, 1);
	self->y = luaL_checkint(L, 2);

	self->width = luaL_checkint(L, 3);
	self->height = luaL_checkint(L, 4);

	return NULL;

}

int quadNew(lua_State *L) { // love.graphics.newQuad()

	int x = luaL_checkint(L, 1);
	int y = luaL_checkint(L, 2);

	int width = luaL_checkint(L, 3);
	int height = luaL_checkint(L, 4);

	love_quad *self = luaobj_newudata(L, sizeof(*self));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	const char *error = quadInit(self, x, y, width, height);

	if (error) luaError(error);

	return 1;
}

int initQuadClass(lua_State *L) {

	luaL_Reg reg[] = {
		{ "new", quadNew },
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, quadNew, reg);

	return 1;

}
