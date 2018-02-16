#include "runtime.h"
#include "event.h"

//Löve2D Functions

//love.event.quit
int Event::Quit(lua_State * L)
{
	LOVE_QUIT = true;

	return 0;
}

//End Löve2D Functions

int Event::Register(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "quit", Quit },
		{ 0, 0}
	};

	luaL_newlib(L, reg);

	return 1;
}