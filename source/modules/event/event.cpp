#include "common/runtime.h"
#include "event.h"

love::Event * love::Event::instance = 0;

using love::Event;

Event::Event() {}

int Event::Quit(lua_State * L)
{
	LOVE_QUIT = true;

	return 0;
}

int eventInit(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "quit",	love::Event::Quit	},
		{ 0, 0 },
	};
	
	luaL_newlib(L, reg);
	
	return 1;
}