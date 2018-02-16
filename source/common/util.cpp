#include "runtime.h"
#include <stdarg.h>

void love_getfield(lua_State * L, const char * field)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "love");
	lua_getfield(L, -1, field);
	lua_remove(L, -2);
}

/*
** Pushes C++-made userdata to the Lua stack
** Credit to Mik Embley for figuring this out
*/
void lua_pushuserdata(lua_State * L, void * userdata, const char * metatable)
{
	void ** object = (void **)lua_newuserdata(L, sizeof(void *));
	*object = userdata;
	luaL_getmetatable(L, metatable);
	lua_setmetatable(L, -2);
}