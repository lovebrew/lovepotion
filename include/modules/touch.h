#pragma once

namespace Touch
{
	//Löve2D Functions

	int GetPosition(lua_State * L);
	
	int GetTouches(lua_State * L);

	int GetPressure(lua_State * L);

	//End Löve2D Functions

	int Register(lua_State * L);
}