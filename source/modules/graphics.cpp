#include "common/runtime.h"
#include "modules/graphics.h"

#include "objects/image/image.h"
#include "objects/image/wrap_image.h"

bool isInitialized = false;
u32 * FRAMEBUFFER;

#define RGB(r, g, b) r | (g << (u32)8) | (b << (u32)16)

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

int Graphics::SetBackgroundColor(lua_State * L)
{
	u32 width, height;
	FRAMEBUFFER = (u32 *)gfxGetFramebuffer((u32*)&width, (u32*)&height);

	int r = luaL_optnumber(L, 1, 255);
	int g = luaL_optnumber(L, 2, 255);
	int b = luaL_optnumber(L, 3, 255);

	for (u32 y = 0; y < height; y++)
	{
		for (u32 x = 0; x < width; x++)
		{
			u32 pos = y * width + x;
			FRAMEBUFFER[pos] = RGBA8_MAXALPHA(r, g, b);
		}
	}

	return 0;
}

int Graphics::Clear(lua_State * L)
{
	gfxFlushBuffers();
	gfxSwapBuffers();
	gfxWaitForVsync();

	return 0;
}

int Graphics::Draw(lua_State * L)
{
	u32 width, height;
	FRAMEBUFFER = (u32 *)gfxGetFramebuffer((u32*)&width, (u32*)&height);

	Image * graphic = (Image *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGE);

	float x = luaL_optnumber(L, 2, 0);
	float y = luaL_optnumber(L, 3, 0);

	u8 * data = graphic->GetImage();

	for (u32 fy = y; fy < height; fy++) //Access the buffer linearly.
	{
		for (u32 fx = x; fx < width; fx++)
		{
			u32 pos = fy * width + fx;
			FRAMEBUFFER[pos] = RGBA8_MAXALPHA(data[pos * 3 + 0], data[pos * 3 + 1], data[pos * 3 + 2]);
		}
	}
	
	return 0;
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
		{ "newImage",			imageNew			},
		{ "draw",				Draw				},
		{ "setBackgroundColor",	SetBackgroundColor	},
		{ "getWidth",			GetWidth			},
		{ "getHeight",			GetHeight			},
		{ "getRendererInfo",	GetRendererInfo		},
		{ 0, 0 }
	};

	luaL_newlib(L, reg);

	return 1;
}