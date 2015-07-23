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

lua_State *L;

char* concat(char *s1, char *s2) {
	char *result = malloc(strlen(s1)+strlen(s2)+1);
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

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

	luaL_dostring(L, concat(concat(concat("love.", tableName), " = "), tableName)); // Ugly, will be replaced.

}

void error(lua_State *L, char *msg) {
	fprintf(stderr, "\nFATAL ERROR:\n  %s: %s\n\n",
		msg, lua_tostring(L, -1));
}

int main() {

	L = luaL_newstate();
	luaL_openlibs(L);

	luaL_dostring(L, "if not love then love = {} end");

	initLoveGraphics(L); // Init modules.
	initLoveTimer(L);

	sf2d_init();
	sf2d_set_clear_color(RGBA8(0x0, 0x0, 0x0, 0xFF));

	consoleInit(GFX_BOTTOM, NULL);

	luaL_dostring(L, "print('love.graphics:')");
	luaL_dostring(L, "table.foreach(love.graphics, print)");
	luaL_dostring(L, "boxx, boxy = 50, 50");

	while (aptMainLoop()) {

		hidScanInput();

		sf2d_start_frame(GFX_TOP, GFX_LEFT);

			if(keysHeld()&KEY_LEFT)luaL_dostring(L, "boxx = boxx - 1");
			if(keysHeld()&KEY_RIGHT)luaL_dostring(L, "boxx = boxx + 1");

			//sf2d_draw_rectangle(50, 50, 100, 100, RGBA8(0xFF, 0x00, 0x00, 0xFF));
			//luaL_dostring(L, "love.graphics.setBackgroundColor(0, 255, 0)");
			luaL_dostring(L, "love.graphics.setColor(0, 0, 255)");
			//luaL_dostring(L, "print(love.graphics.getColor())");
			luaL_dostring(L, "love.graphics.rectangle('fill', boxx, boxy, 50, 50)");
			luaL_dostring(L, "love.graphics.setColor(255, 0, 0)");
			luaL_dostring(L, "love.graphics.circle('fill', 100, 100, 50, 50)");
		sf2d_end_frame();

		// sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		// sf2d_end_frame();

		sf2d_swapbuffers();

	}

	sf2d_fini();

	lua_close(L);

	return 0;

}