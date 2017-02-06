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
#include "boot_lua.h"
#include "nogame_lua.h"

char *rootDir = "";

lua_State *L;

int initLove(lua_State *L);

bool romfsExists;

bool errorOccured = false;
bool forceQuit = false;
const char *errMsg;
bool irrstEnabled = false;

char * identity = "SuperGame";
char * path = "sdmc:/LovePotion/";

float updateDelay = STREAM_RATE;

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
	
	Result ir = irrstInit();

	consoleInit(GFX_BOTTOM, NULL);

	irrstEnabled = (ir) ? false : true;

	sf2d_set_clear_color(RGBA8(0x0, 0x0, 0x0, 0xFF)); // Reset background color.

	osSetSpeedupEnable(true); // Enables CPU speedup for a free performance boost.

	// Detect if we are running on a .cia, because if we are
	// we load from RomFS rather than the SD Card.
	// TODO: Load RomFS from .3dsx's aswell.

	Result rc = romfsInit();

	romfsExists = (rc) ? false : true;

	// Change working directory

	if (romfsExists) {

		chdir("romfs:/");

	} else {

		char cwd[256];
		getcwd(cwd, 256);
		char newCwd[261];

		strcat(newCwd, cwd);
		strcat(newCwd, "game");
		chdir(newCwd);

	}

	luaL_dobuffer(L, boot_lua, boot_lua_size, "boot"); // Do some setup Lua side.

	//in general, make the LovePotion folder cause ye
	mkdir(path, 0777);

	// If main.lua exists, execute it.
	// If not then just load the nogame screen.

	if (fileExists("main.lua")) {
		if (luaL_dofile(L, "main.lua")) displayError();
	} else {
		if (luaL_dobuffer(L, nogame_lua, nogame_lua_size, "nogame")) displayError();
	}
	
	if (luaL_dostring(L, "love.timer.step()")) displayError();

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


			//update streams
			if (updateDelay > 0) {
				updateDelay -= dt;
			} else {
				updateStreams();
				updateDelay = STREAM_RATE;
			}

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

			//sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);

			//	if (luaL_dostring(L, "if love.draw then love.draw() end")) displayError();

			//sf2d_end_frame();

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
	
	if (irrstEnabled) irrstExit();

	if (soundEnabled) ndspExit();
	if (romfsExists) romfsExit();

	return 0;

}
