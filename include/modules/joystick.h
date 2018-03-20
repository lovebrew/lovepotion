/*
** modules/joystick.h
** @brief	: Handles (basic) joystick stuff.
** @ref		: See wrap_gamepad.cpp for advanced use.
*/

#pragma once

namespace Joystick
{
	void Initialize(); //should set up joysticks

	//Löve2D Functions

	int GetJoysticks(lua_State * L);

	int GetJoystickCount(lua_State * L);

	int SetJoyconMode(lua_State * L);

	//End Löve2D Functions

	int Register(lua_State * L);
}