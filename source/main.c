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

#include "include/lua/lua.h"
#include "include/lua/lualib.h"
#include "include/lua/lauxlib.h"

#include "shared.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>

#include "logo_png.h"

lua_State *L;

char* concat(char *s1, char *s2) {
	char *result = malloc(strlen(s1)+strlen(s2)+1);
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

// Stolen from some StackOverflow question, doesn't work as it should, needs replacing.

void registerFunction(char const * const tableName, char const * const funcName, void (*funcPointer)) {

	lua_getfield(L, LUA_GLOBALSINDEX, tableName); // push table onto stack
	if (!lua_istable(L, -1)) {
		lua_createtable(L, 0, 1); // create new table
		lua_setfield(L, LUA_GLOBALSINDEX, tableName); // add it to global context

		// reset table on stack
		lua_pop(L, 1); // pop table (nil value) from stack
		lua_getfield(L, LUA_GLOBALSINDEX, tableName); // push table onto stack
	}

	lua_pushstring(L, funcName); // push key onto stack
	lua_pushcfunction(L, funcPointer); // push value onto stack
	lua_settable(L, -3); // add key-value pair to table

	lua_pop(L, 1); // pop table from stack

	luaL_dostring(L, concat(concat(concat("love.", tableName), " = "), tableName)); // Ew.

}

int main() {

	L = luaL_newstate();
	luaL_openlibs(L);

	luaL_dostring(L, "if not love then love = {} end");

	initLoveGraphics(L); // Init modules.
	initLoveTimer(L);
	initLoveSystem(L);
	initLoveKeyboard(L);

	sf2d_init();
	sf2d_set_clear_color(RGBA8(0x0, 0x0, 0x0, 0xFF));

	consoleInit(GFX_BOTTOM, NULL);

	luaL_dostring(L, "print(''); print('\x1b[1;36mLovePotion 0.0.1\x1b[0m (Love2D for 3DS)'); print('')"); // Ew again.
	//luaL_dostring(L, "print('love.graphics:')");
	//luaL_dostring(L, "table.foreach(love.graphics, print)");

	//luaL_dostring(L, script);

	luaL_dofile(L, "LovePotion/main.lua");
	luaL_dostring(L, "if love.load then love.load() end");

	sf2d_texture *tex2 = sfil_load_PNG_buffer(logo_png, SF2D_PLACE_RAM);

	while (aptMainLoop()) {

		luaL_dostring(L, "love.keyboard.scan()");

		luaL_dostring(L, "love.timer.step()");

		luaL_dostring(L, "if love.update then love.update(love.timer.getDelta()) end"); // TODO: Pass delta-time.

		sf2d_start_frame(GFX_TOP, GFX_LEFT);

			luaL_dostring(L, "if love.draw then love.draw() end");

			//sf2d_draw_texture(tex2, 400/2 - tex2->width/2, 240/2 - tex2->height/2);

		sf2d_end_frame();

		// TODO: Work on being able to draw to both screens.

		// sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		// sf2d_end_frame();

		luaL_dostring(L, "love.graphics.present()");

	}

	sf2d_fini();

	lua_close(L);

	return 0;

}