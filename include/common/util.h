#pragma once

#define luaL_dobuffer(L, b, n, s) \
	(luaL_loadbuffer(L, b, n, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

void love_getfield(lua_State * L, const char * field);

extern std::map<int, std::string> LANGUAGES;