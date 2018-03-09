#include "common/runtime.h"
#include "modules/graphics.h"

#include "objects/image/image.h"
#include "objects/image/wrap_image.h"

#include "objects/quad/quad.h"
#include "objects/quad/wrap_quad.h"

bool isInitialized = false;
u32 * FRAMEBUFFER;
u32 backgroundColor = 0xFF000000;

#define RGBA(r, g, b, a) r | (g >> (u32)8) | (b >> (u32)16) | (a >> (u32)24)

void Graphics::Initialize()
{
	if (!Console::IsInitialized())
		gfxInitDefault();

	isInitialized = true;
}

void renderto(u32* target, u32 pos, u8 r, u8 g, u8 b, u8 a)
{ // for adding alpha values and such
	u32 currentColor = target[pos];
	double curR = (double)(currentColor & 0x000000FF);
	double curG = (double)(currentColor & 0x0000FF00 >> 8);
	double curB = (double)(currentColor & 0x00FF0000 >> 16);
	double curA = (double)(currentColor & 0xFF000000 >> 24) / 255.0;

	double newR = (double)r;
	double newG = (double)g;
	double newB = (double)b;
	double newA = (double)a / 255.0;

	u8 outR, outG, outB, outA;

	if ((curA * 255) > 0 && (newA * 255) > 0) // check if both alpha channels exist
	{
		double invA = (1.0 - newA);
		double mixA = (1.0 - invA * (1.0 - curA));
		invA = curA * invA / mixA;
		newA = newA / mixA;
		outR = curR * invA + newR * newA;
		outG = curG * invA + newG * newA;
		outB = curB * invA + newB * newA;
		outA = mixA * 255;
	}
	else if ((newA * 255) > 0) // if no color exists, add some
	{
		outR = newR;
		outG = newG;
		outB = newB;
		outA = newA;
	}
	else // if theres no color (alpha==0) to add, keep the color there
		return;

	target[pos] = RGBA8(outR, outG, outB, outA);
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
	double r = luaL_optnumber(L, 1, 0);
	double g = luaL_optnumber(L, 2, 0);
	double b = luaL_optnumber(L, 3, 0);

	u8 outR, outG, outB;

	outR = r * 255;
	outG = g * 255;
	outB = b * 255;

	backgroundColor = RGBA8(outR, outG, outB, 255);

	return 0;
}

int Graphics::GetBackgroundColor(lua_State * L)
{
	u8 r = (backgroundColor & 0x000000FF);
	u8 g = (backgroundColor & 0x0000FF00 >> 8);
	u8 b = (backgroundColor & 0x00FF0000 >> 16);

	lua_pushnumber(L, r);
	lua_pushnumber(L, g);
	lua_pushnumber(L, b);

	return 3;
}

int Graphics::Clear(lua_State * L)
{
	u32 width, height;
	FRAMEBUFFER = (u32 *)gfxGetFramebuffer((u32*)&width, (u32*)&height);

	for (u32 y = 0; y < height; y++)
	{
		for (u32 x = 0; x < width; x++)
		{
			u32 pos = y * width + x;

			FRAMEBUFFER[pos] = backgroundColor;
		}
	}

	return 0;
}

int Graphics::Present(lua_State * L)
{
	gfxSwapBuffers();
	gfxWaitForVsync();
	gfxFlushBuffers();
	
	return 0;
}

int Graphics::Draw(lua_State * L)
{
	u32 screenwidth, screenheight;
	gfxGetFramebuffer((u32*)&screenwidth, (u32*)&screenheight);

	Image * graphic = (Image *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGE);
	Quad * quad = nullptr;

	int start = 2;
	if (!lua_isnoneornil(L, 2) && !lua_isnumber(L, 2))
	{
		quad = (Quad *)luaobj_checkudata(L, 2, LUAOBJ_TYPE_QUAD);
		start = 3;
	}

	float x = luaL_optnumber(L, start + 0, 0);
	float y = luaL_optnumber(L, start + 1, 0);

	vector<u8> data = graphic->GetImage();
	u16 width = graphic->GetWidth();
	u16 height = graphic->GetHeight();

	u32 pos, color;
	for (u16 fy = 0; fy < height; fy++) //Access the buffer linearly.
	{
		if ((fy + y) < 0.0) // above screen, skip draw until on screen
			continue;
		if ((fy + y) >= screenheight) // below screen, end draw
			break;

		for (u16 fx = 0; fx < width; fx++)
		{
			if ((fx + x) < 0.0) // outside the screen on the left
				continue;
			if ((fx + x) >= screenwidth) // outside the screen on the right
				break;

			pos = (fy + y) * screenwidth + fx + x;

			color = (fy * width + fx) * 4;
			renderto(FRAMEBUFFER, pos, data[color + 0], data[color + 1], data[color + 2], data[color + 3]);
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
		{ "newQuad",			quadNew				},
		{ "draw",				Draw				},
		{ "clear",				Clear				},
		{ "present",			Present				},
		{ "setBackgroundColor",	SetBackgroundColor	},
		{ "getBackgroundColor",	GetBackgroundColor	},
		{ "getWidth",			GetWidth			},
		{ "getHeight",			GetHeight			},
		{ "getRendererInfo",	GetRendererInfo		},
		{ 0, 0 }
	};

	luaL_newlib(L, reg);

	return 1;
}
