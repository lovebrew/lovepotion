#include "runtime.h"
#include "window.h"

//Löve2D Functions

//love.window.setMode
int Window::SetMode(lua_State * L)
{
	return 0;
}

//End Löve2D Functions

int Window::Register(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "setMode", SetMode },
		{ 0, 0}
	};

	luaL_newlib(L, reg);

	return 1;
}