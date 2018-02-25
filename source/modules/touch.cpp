#include "common/runtime.h"
#include "modules/touch.h"

int Touch::GetPosition(lua_State * L)
{
	if (lua_islightuserdata(L, 1))
	{
		touchPosition touch;

		int * id = (int *)lua_topointer(L, 1);
		
		hidTouchRead(&touch, *id);

		lua_pushinteger(L, touch.px);
		lua_pushinteger(L, touch.py);

		return 2;
	}

	return 0;
}

int Touch::GetTouches(lua_State * L)
{
	lua_newtable(L);

	for (u32 i = 0; i < hidTouchCount(); i++)
	{
		lua_pushnumber(L, i);
		lua_pushnumber(L, i);

		lua_settable(L, -3);
	}

	return 1;
}

int Touch::Register(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "getPosition",	GetPosition	},
		{ "getTouches",		GetTouches	},
		{ 0, 0 }
	};

	luaL_newlib(L, reg);
	
	return 1;
}