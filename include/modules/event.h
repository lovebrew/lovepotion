/*
** modules/event.h
** @brief	: Handles events.. like quitting.. and other things
*/

#pragma once

namespace Event
{
	void JoystickEvent(lua_State * L, SDL_Event event);

	//Löve2D Functions

	int Quit(lua_State * L);

	int Poll(lua_State * L);

	//End Löve2D Functions

	int Register(lua_State * L);
}