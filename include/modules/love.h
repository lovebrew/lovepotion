/*
** modules/love.h
** @brief	: Handles all the modules
*/

#pragma once

extern bool LUA_ERROR;
extern bool CLOSE_KEYBOARD;

namespace Love
{
	int Initialize(lua_State * L);

	int GetVersion(lua_State * L);

	int EnableConsole(lua_State * L);

	bool IsRunning();
	
	int Scan(lua_State * L);

	int Run(lua_State * L);

	void GamepadAxis(lua_State * L, Gamepad * controller, std::pair<std::string, float> & data);

	void GamepadPressed(lua_State * L, Gamepad * controller);

	void GamepadReleased(lua_State * L, Gamepad * controller);

	void TouchPressed(lua_State * L);

	void TouchMoved(lua_State * L);

	void TouchReleased(lua_State * L);

	int NoGame(lua_State * L);

	void Exit(lua_State * L);
}

// Forward declare lua_State.
struct lua_State;