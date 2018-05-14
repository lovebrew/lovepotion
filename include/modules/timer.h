/*
** timer.h
** @brief : Used for time keeping
*/

#pragma once

namespace Timer
{
	//Löve2D Functions

	int GetFPS(lua_State * L);
	int GetTime(lua_State * L);
	int GetDelta(lua_State * L);
	int Sleep(lua_State * L);
	int Step(lua_State * L);

	//End Löve2D Functions

	void Tick();

	int Register(lua_State * L);
}