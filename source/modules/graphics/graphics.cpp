#include "common/runtime.h"
#include "graphics.h"

love::Graphics * love::Graphics::instance = 0;

using love::Graphics;

gfxScreen_t Graphics::currentScreen = GFX_TOP;

Graphics::Graphics() {}

int Graphics::GetWidth(lua_State * L)
{
	int width = 400;
	if (currentScreen == GFX_BOTTOM)
		width = 320;

	lua_pushnumber(L, width);

	return 1;
}

int Graphics::GetHeight(lua_State * L)
{
	lua_pushnumber(L, 240);

	return 1;
}

int Graphics::SetScreen(lua_State * L)
{
	const char * screen = luaL_checkstring(L, 1);

	if (strncmp(screen, "top", 3) == 0)
		currentScreen = GFX_TOP;
	else if (strncmp(screen, "bottom", 6) == 0)
		currentScreen = GFX_BOTTOM;

	return 0;
}

int graphicsInit(lua_State * L)
{	
	gfxInitDefault();
	gfxSet3D(false);

	C3D_Init(0x80000 * 8);

	luaL_Reg reg[] = 
	{
		{ "getWidth",	love::Graphics::GetWidth	},
		{ "getHeight",	love::Graphics::GetHeight	},
		{ "setScreen",	love::Graphics::SetScreen	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}