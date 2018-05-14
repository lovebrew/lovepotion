#include "runtime.h"
#include "mouse.h"

//Löve2D Functions

int Mouse::GetX(lua_State * L)
{
	lua_pushnumber(L, lastTouch[0]);
	
	return 1;
}

int Mouse::GetY(lua_State * L)
{
	lua_pushnumber(L, lastTouch[1]);

	return 1;
}

int Mouse::GetPosition(lua_State * L)
{
	lua_pushnumber(L, lastTouch[0]);
	lua_pushnumber(L, lastTouch[1]);

	return 2;
}

int Mouse::IsDown(lua_State * L)
{
	lua_pushboolean(L, touchDown);

	return 1;
}

//End Löve2D Functions

int Mouse::Register(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "getX",			GetX			},
		{ "getY",			GetY			},
		{ "getPosition",	GetPosition		},
		{ "isDown",			IsDown			},
		{ 0, 0 }
	};

	luaL_newlib(L, reg);

	return 1;
}