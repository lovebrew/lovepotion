#include "common/runtime.h"
#include "mouse.h"

love::Mouse * love::Mouse::instance = 0;

using love::Mouse;

int Mouse::GetX(lua_State * L)
{
	int touchX = touch.px;
	if (!touchDown)
		touchX = lastTouch[0];

	lua_pushinteger(L, touchX);

	return 1;
}

int Mouse::GetY(lua_State * L)
{
	int touchY = touch.py;
	if (!touchDown)
		touchY = lastTouch[1];

	lua_pushinteger(L, touchY);
	
	return 1;
}

int Mouse::IsDown(lua_State * L)
{
	lua_pushboolean(L, touchDown);

	return 1;
}

int mouseInit(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{"getX",	love::Mouse::GetX	},
		{"getY",	love::Mouse::GetY	},
		{"isDown",	love::Mouse::IsDown	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}