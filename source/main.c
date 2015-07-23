#include "include/lua/lua.h"
#include "include/lua/lualib.h"
#include "include/lua/lauxlib.h"

#include "shared.h"

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>

lua_State *L;

void error(lua_State *L, char *msg) {
	fprintf(stderr, "\nFATAL ERROR:\n  %s: %s\n\n",
		msg, lua_tostring(L, -1));
}

int main(int argc, char **argv) {

	L = luaL_newstate();
	luaL_openlibs(L);

	gfxInitDefault();

	consoleInit(GFX_BOTTOM, NULL);

	initLoveGraphics(L);

	luaL_dostring(L, "for i=1, 5 do print('Hello, world!') end");

	//printf("Hello, World!");

	while (aptMainLoop()) {

		hidScanInput();

		u32 kDown = hidKeysDown();

		if (kDown & KEY_UP) break;

		gfxFlushBuffers();
		gfxSwapBuffers();

		gspWaitForVBlank();

	}

	gfxExit();

	lua_close(L);

	return 0;

}