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

char *rootDir = "";

lua_State *L;

int initLove(lua_State *L);

bool isCIA;

bool errorOccured = false;
bool forceQuit = false;
char *errMsg;

void displayError() {

	errMsg = lua_tostring(L, -1);

	errorOccured = true;
	printf("\e[0;31m%s\e[0m",errMsg);

}

int main() {

	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requiref(L, "love", initLove, 1);

	sf2d_init(); // 2D Drawing lib.
	sftd_init(); // Text Drawing lib.
	cfguInit();
	ptmuInit();

	//consoleInit(GFX_BOTTOM, NULL);

	sf2d_set_clear_color(RGBA8(0x0, 0x0, 0x0, 0xFF)); // Reset background color.

	osSetSpeedupEnable(true); // Enables CPU speedup for a free performance boost.

	// Detect if we are running on a .cia, because if we are
	// we load from RomFS rather than the SD Card.

	Result rc = romfsInit();

	if (rc) {
		isCIA = false;
	} else {
		isCIA = true;
	}

	// Change working directory

	if (isCIA) {

		chdir("romfs:/");

	} else {

		char cwd[256];
		getcwd(cwd, 256);
		char newCwd[261];

		strcat(newCwd, cwd);
		strcat(newCwd, "game");
		chdir(newCwd);

	}

	luaL_dostring(L, "_defaultFont_ = love.graphics.newFont(); love.graphics.setFont(_defaultFont_)");

	luaL_dostring(L, "print(''); print('\x1b[1;36mLovePotion 1.0.9 BETA\x1b[0m (LOVE for 3DS)'); print('')"); // Ew.

	luaL_dostring(L, "package.path = './?.lua;./?/init.lua'"); // Set default requiring path.
	luaL_dostring(L, "package.cpath = './?.lua;./?/init.lua'");

	luaL_dostring(L, 
		"function love.errhand(msg)\n"
			"love.audio.stop()"
			"love.graphics.setBackgroundColor(89, 157, 220)\n"
			"love.graphics.setScreen('top')\n"
			"love.graphics.setFont(_defaultFont_)\n"
			"love.graphics.setColor(255, 255, 255, 255)\n"
			"love.graphics.print('Oops, a Lua error has occured', 25, 25)\n"
			"love.graphics.print('Press Start to quit', 25, 40)\n"
			"love.graphics.printf(msg, 25, 70, love.graphics.getWidth() - 50)\n"
		"end"
	); // default love.errhand()

	if (luaL_dofile(L, "main.lua")) displayError();

	if (luaL_dostring(L, "if love.load then love.load() end")) displayError();

	while (aptMainLoop()) {

		if (shouldQuit) {

			if (forceQuit) break;

			bool shouldAbort = false;

			// lua_getfield(L, LUA_GLOBALSINDEX, "love");
			// lua_getfield(L, -1, "quit");
			// lua_remove(L, -2);

			// if (!lua_isnil(L, -1)) {

			// 	lua_call(L, 0, 1);
			// 	shouldAbort = lua_toboolean(L, 1);
			// 	lua_pop(L, 1);

			// }; TODO: Do this properly.

			if (luaL_dostring(L, "if love.quit then love.quit() end")) displayError();

			if (!shouldAbort && !errorOccured) break;

		} // Quit event

		if (!errorOccured) {

			if (luaL_dostring(L,
				"love.keyboard.scan()\n"
				"love.timer.step()\n"
				"if love.update then love.update(love.timer.getDelta()) end")) {
					displayError();
			}

			//Update sources that stream or something
			//for (int i = 0; i < streamCount; i++) {
			//	sourceUpdate(L, streams[i]);
			//}

			// Top screen
			// Left side

			sf2d_start_frame(GFX_TOP, GFX_LEFT);

				if (luaL_dostring(L, "if love.draw then love.draw() end")) displayError();

			sf2d_end_frame();

			// Right side

			if (is3D) {

				sf2d_start_frame(GFX_TOP, GFX_RIGHT);

					if (luaL_dostring(L, "if love.draw then love.draw() end")) displayError();

				sf2d_end_frame();

			}

			// Bot screen

			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);

				if (luaL_dostring(L, "if love.draw then love.draw() end")) displayError();

			sf2d_end_frame();

			luaL_dostring(L, "love.graphics.present()");

		} else {

			hidScanInput();
			u32 kTempDown = hidKeysDown();
			if (kTempDown & KEY_START) {
				forceQuit = true;
				shouldQuit = true;
			}

			char *errMsg = lua_tostring(L, -1);

			sf2d_start_frame(GFX_TOP, GFX_LEFT);

				lua_getfield(L, LUA_GLOBALSINDEX, "love");
				lua_getfield(L, -1, "errhand");
				lua_remove(L, -2);

				if (!lua_isnil(L, -1)) {

					lua_pushstring(L, errMsg);
					lua_call(L, 1, 0);

				}

			sf2d_end_frame();

			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);

				lua_getfield(L, LUA_GLOBALSINDEX, "love");
				lua_getfield(L, -1, "errhand");
				lua_remove(L, -2);

				if (!lua_isnil(L, -1)) {

					lua_pushstring(L, errMsg);
					lua_call(L, 1, 0);

				}

			sf2d_end_frame();

			luaL_dostring(L, "love.graphics.present()");

		}

	}

	luaL_dostring(L, "love.audio.stop()");

	lua_close(L);

	sftd_fini();
	sf2d_fini();
	cfguExit();
	ptmuExit();

	if (soundEnabled) ndspExit();
	if (isCIA) romfsExit();

	return 0;

}
