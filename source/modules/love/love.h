#pragma once

// Forward declare lua_State.
extern lua_State * L;
extern bool LUA_ERROR;
extern bool CLOSE_KEYBOARD;

/*
	@brief returns the LOVE version :: Lua
	@return MAJOR, MINOR, REVISION, and CODENAME
*/
int loveGetVersion(lua_State * L);

int loveScan(lua_State * L);

int loveMouseMoved(lua_State * L);

int loveEnableConsole(lua_State * L);

int loveNoGame(lua_State * L);

void loveChangeDir(bool isFused);

void loveCreateSaveDirectory();

int loveEnableScreen(lua_State * L);

/*
	@brief initializes LOVE, its modules, and their classes
*/
int loveInit(lua_State * L);

/*
	@brief closes LOVE's modules (and services they use)
*/
void loveClose(lua_State * L);