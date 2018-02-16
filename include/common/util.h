#define luaL_dobuffer(L, b, n, s) \
	(luaL_loadbuffer(L, b, n, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

void lua_pushuserdata(lua_State * L, void * userdata, const char * metatable);

void love_getfield(lua_State * L, const char * field);