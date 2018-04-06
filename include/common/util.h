#pragma once

#define luaL_dobuffer(L, b, n, s) \
	(luaL_loadbuffer(L, b, n, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

#define LOVE_TIMER_STEP "if love.timer then love.timer.step() end"

#define LOVE_UPDATE		"if love.update then love.update(love.timer.getDelta()) end"

#define LOVE_DRAW		"if love.draw then love.draw() end"

#define LOVE_CLEAR		"love.graphics.clear()"

#define LOVE_PRESENT	"love.graphics.present()"

#include <exception>

void love_getfield(lua_State * L, const char * field);

int love_preload(lua_State * L, lua_CFunction function, const char * name);

double clamp(double low, double x, double high);

extern FILE * logFile;

extern std::vector<std::string> KEYS;

extern std::map<int, std::string> LANGUAGES;

void logToFile(const std::string & data);

void closeLog();

/**
 * Converts any exceptions thrown by the passed lambda function into a Lua error.
 * lua_error (and luaL_error) cannot be called from inside the exception handler
 * because they use longjmp, which causes undefined behaviour when the
 * destructor of the exception would have been called.
**/
template <typename T>
int lua_catchexception(lua_State * L, const T & func)
{
	bool should_error = false;

	try
	{
		func();
	}
	catch (const std::exception & e)
	{
		should_error = true;
	
		lua_pushstring(L, e.what());
	}

	if (should_error)
		return luaL_error(L, "%s", lua_tostring(L, -1));
	
	return 0;
}