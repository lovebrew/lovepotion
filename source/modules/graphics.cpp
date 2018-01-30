#include "runtime.h"
#include "graphics.h"

#include <switch.h>

bool isInitialized = false;

void Graphics::Initialize()
{
	if (!Console::IsInitialized())
		gfxInitDefault();

	isInitialized = true;
}

//Löve2D Functions

//love.graphics.getWidth
int Graphics::GetWidth(lua_State * L)
{
	return 1280;
}

//love.graphics.getHeight
int Graphics::GetHeight(lua_State * L)
{
	return 720;
}

//End Löve2D Functions

bool Graphics::IsInitialized()
{
	return isInitialized;
}

void Graphics::Exit()
{
	gfxExit();
}

int Graphics::Register(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "getWidth",	GetWidth	},
		{ "getHeight",	GetHeight	},
		{ 0, 0 }
	};

	luaL_newlib(L, reg);

	return 1;
}