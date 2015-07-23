#include "../include/lua/lua.h"
#include "../include/lua/lualib.h"
#include "../include/lua/lauxlib.h"

#include "../shared.h"

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>

static int test(lua_State *L) {

	printf("This is a test.");

}

int initLoveGraphics(lua_State *L) {
	lua_register(L, "test", test);
	return 0;
}