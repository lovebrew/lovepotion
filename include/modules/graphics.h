/*
** graphics.h
** @brief : Used for rendering graphics (primitives, images, text)
*/

#pragma once

namespace Graphics
{
	void Initialize();
	bool IsInitialized();

	//Löve2D Functions

	int GetWidth(lua_State * L);
	int GetHeight(lua_State * L);

	int Register(lua_State * L);

	void Exit();
}