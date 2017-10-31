#include "common/runtime.h"
#include "window.h"

love::Window * love::Window::instance = 0;

using love::Window;

int Window::SetMode(lua_State * L)
{
	return 0;
}

int windowInit(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "setMode",	love::Window::SetMode	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}