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

int mouseIsDown(lua_State *L) { // love.mouse.isDown()

	lua_pushboolean(L, touchIsDown);

	return 1;

}

int mouseGetX(lua_State *L) { // love.mouse.getX()

	lua_pushinteger(L, touch.px);

	return 1;

}

int mouseGetY(lua_State *L) { // love.mouse.getY()

	lua_pushinteger(L, touch.py);

	return 1;

}

int mouseGetPosition(lua_State *L) { // love.mouse.getPosition()

	lua_pushinteger(L, touch.px);
	lua_pushinteger(L, touch.py);

	return 2;

}

int initLoveMouse(lua_State *L) {

	luaL_Reg reg[] = {
		{ "isDown",			mouseIsDown			},
		{ "getX",			mouseGetX			},
		{ "getY",			mouseGetY			},
		{ "getPosition",	mouseGetPosition	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);

	return 1;

}