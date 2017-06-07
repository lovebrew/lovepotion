#pragma once

// Forward declare lua_State.
struct lua_State;
extern bool LUA_ERROR;

/*
	@brief returns the LOVE version :: Lua
	@return MAJOR, MINOR, REVISION, and CODENAME
*/
int love_getVersion(lua_State * L);

int loveConf(lua_State * L);

/*
	@brief initializes LOVE, its modules, and their classes
*/
int loveInit(lua_State * L);