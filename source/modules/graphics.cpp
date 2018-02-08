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
	lua_pushnumber(L, 1280);

	return 1;
}

//love.graphics.getHeight
int Graphics::GetHeight(lua_State * L)
{
	lua_pushnumber(L, 720);
	
	return 1;
}

int Graphics::GetRendererInfo(lua_State * L)
{
	lua_pushstring(L, "OpenGL");
	lua_pushstring(L, "4.5");
	lua_pushstring(L, "NVIDIA");
	lua_pushstring(L, "TEGRA X1");
 
	return 4;
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
		{ "getWidth",			GetWidth		},
		{ "getHeight",			GetHeight		},
		{ "getRendererInfo",	GetRendererInfo	},
		{ 0, 0 }
	};

	luaL_newlib(L, reg);

	return 1;
}