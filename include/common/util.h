#pragma once

#define luaL_dobuffer(L, b, n, s) \
	(luaL_loadbuffer(L, b, n, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

#define LOVE_TIMER_STEP "if love.timer then " \
							"love.timer.step() " \
						"end"

#define LOVE_UPDATE		"if love.update then " \
							"love.update(love.timer.getDelta()) " \
						"end"

#define LOVE_DRAW		"if love.draw then " \
							"love.draw() " \
						"end"

void love_getfield(lua_State * L, const char * field);

extern std::map<int, std::string> LANGUAGES;