#include "include/lua/lua.h"
#include "include/lua/lualib.h"
#include "include/lua/lauxlib.h"

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

	lua_State *L;

	L = luaL_newstate();
	luaL_openlibs(L);

	gfxInitDefault();

	consoleInit(GFX_BOTTOM, NULL);

	printf("Hello, World!");

	while (aptMainLoop()) {

		hidScanInput();

		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break;

		gfxFlushBuffers();
		gfxSwapBuffers();

		gspWaitForVBlank();

	}

	lua_close(L);

}