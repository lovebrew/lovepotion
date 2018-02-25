#include "common/runtime.h"
#include <stdarg.h>

void love_getfield(lua_State * L, const char * field)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "love");
	lua_getfield(L, -1, field);
	lua_remove(L, -2);
}