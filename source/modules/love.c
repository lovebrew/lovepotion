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

#define LOVE_VERSION "0.10.1"

static int loveGetVersion(lua_State *L) { // love.getVersion()

	lua_pushstring(L, LOVE_VERSION);
	return 1;

}

int initLoveSystem(lua_State *L);
int initLoveGraphics(lua_State *L);
int initLoveTimer(lua_State *L);
int initLoveKeyboard(lua_State *L);
int initLoveMouse(lua_State *L);
int initLoveJoystick(lua_State *L);
int initLoveWindow(lua_State *L);
int initLoveEvent(lua_State *L);
int initLoveAudio(lua_State *L);
int initLoveFilesystem(lua_State *L);
int initSocket(lua_State * L);

int initImageClass(lua_State *L);
int initFontClass(lua_State *L);
int initSourceClass(lua_State *L);
int initFileClass(lua_State *L);
int initQuadClass(lua_State *L);
int initJoystickClass(lua_State *L);
int initSocketClass(lua_State *L);
int initCanvasClass(lua_State *L);

int initLove(lua_State *L) {

	// Thanks to rxi for this https://github.com/rxi/lovedos/blob/master/src/love.c

	int i;

	int (*classes[])(lua_State *L) = {
		initImageClass,
		initFontClass,
		initSourceClass,
		initQuadClass,
		initFileClass,
        initJoystickClass,
		initSocketClass,
		initCanvasClass,
		NULL,
	};

	for (i = 0; classes[i]; i++) {
		classes[i](L);
		lua_pop(L, 1);
	}

	luaL_Reg reg[] = {
		{ "getVersion",	loveGetVersion },
		{ 0, 0 },
	};

	luaL_newlib(L, reg);

	struct { char *name; int (*fn)(lua_State *L); } mods[] = {
		{ "system",   initLoveSystem    },
		{ "graphics", initLoveGraphics  },
		{ "timer",    initLoveTimer     },
		{ "keyboard", initLoveKeyboard  },
		{ "mouse",    initLoveMouse     },
        { "joystick", initLoveJoystick  },
		{ "window",   initLoveWindow    },
		{ "event",    initLoveEvent     },
		{ "audio",    initLoveAudio     },
		{ "filesystem",    initLoveFilesystem     },
		{ 0 },
	};

	for (i = 0; mods[i].name; i++) {
		mods[i].fn(L);
		lua_setfield(L, -2, mods[i].name);
	}

	return 1;

}
