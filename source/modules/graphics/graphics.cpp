#include "common/runtime.h"
#include "crendertarget.h"

#include "graphics.h"
#include "image.h"
#include "file.h"
#include "quad.h"
#include "glyph.h"
#include "font.h"

#include "wrap_image.h"
#include "wrap_font.h"
#include "wrap_quad.h"
#include "wrap_graphics.h"

#include "basic_shader_shbin.h"

love::Graphics * love::Graphics::instance = 0;

using love::Graphics;
using love::CRenderTarget;
using love::Image;
using love::Font;
using love::Quad;

int projection_desc = -1;

DVLB_s * dvlb = nullptr;
shaderProgram_s shader;

gfxScreen_t Graphics::currentScreen = GFX_TOP;
gfxScreen_t Graphics::renderScreen = GFX_TOP;

u32 Graphics::backgroundColor = 0x000000FF; //0xRRGGBBAA


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

int Graphics::SetColor(lua_State * L)
{
	int r = luaL_checknumber(L, 1);
	int g = luaL_checknumber(L, 2);
	int b = luaL_checknumber(L, 3);
	int a = NULL;

	if (!lua_isnil(L, 4) && lua_isnumber(L, 4))
	{	
		a = luaL_checknumber(L, 4);
		graphicsSetColor(r, g, b, a);
	}
	else
	{
		graphicsSetColor(r, g, b);
	}

	return 0;
}

int Graphics::Line(lua_State * L)
{	
	float startx = luaL_checknumber(L, 1);
	float starty = luaL_checknumber(L, 2);

	float endx = luaL_checknumber(L, 3);
	float endy = luaL_checknumber(L, 4);

	if (currentScreen == renderScreen)
		graphicsLine(startx, starty, endx, endy);
	
	return 0;
}

int Graphics::Rectangle(lua_State * L)
{
	const char * mode = luaL_checkstring(L, 1);

	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);

	float width = luaL_checknumber(L, 4);
	float height = luaL_checknumber(L, 5);

	if (currentScreen == renderScreen)
		graphicsRectangle(x, y, width, height);

	return 0;
}

int Graphics::Circle(lua_State * L)
{
	const char * mode = luaL_checkstring(L, 1);

	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);

	float radius = luaL_checknumber(L, 4);
	float segments = luaL_optnumber(L, 5, 100);

	if (currentScreen == renderScreen)
		graphicsCircle(x, y, radius, segments);

	return 0;
}

int Graphics::Draw(lua_State * L)
{
	Image * image = (Image *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGE);
	Quad * quad = nullptr;

	int start = 2;
	if (!lua_isnoneornil(L, 2) && !lua_isnumber(L, 2))
	{
		quad = (Quad *)luaobj_checkudata(L, 2, LUAOBJ_TYPE_QUAD);
		start = 3;
	}

	float x = luaL_optnumber(L, start, 0);
	float y = luaL_optnumber(L, start + 1, 0);

	if (currentScreen == renderScreen)
	{
		bindTexture(image->GetTexture());
		if (quad == nullptr)
			graphicsDraw(image->GetTexture(), x, y, image->GetWidth(), image->GetHeight());
		else
			graphicsDrawQuad(image->GetTexture(), x, y, quad->GetX(), quad->GetY(), quad->GetWidth(), quad->GetHeight());
	}
	
	return 0;
}

int Graphics::Points(lua_State * L)
{
	int args = lua_gettop(L);

	bool tableOfTables = false;

	if (args == 1 && lua_istable(L, 1))
	{
		args = lua_objlen(L, 1);

		lua_rawgeti(L, 1, 1);
		tableOfTables = lua_istable(L, -1);
		lua_pop(L, 1);
	}

	if (args % 2 != 0 && !tableOfTables)
		luaL_error(L, "Points must be a multiple of two");

	int points = args / 2;
	if (tableOfTables)
		points = args;

	float * coordinates = new float[points * 2];
	if (lua_istable(L, 1))
	{
		if (!tableOfTables)
		{
			for (int i = 0; i < args; i++)
			{
				lua_rawgeti(L, 1, i + 1);

				coordinates[i] = luaL_checknumber(L, -1);
			
				lua_pop(L, 1);
			}
		}
		else
		{
			for (int i = 0; i < args; i++)
			{
				lua_rawgeti(L, 1, i + 1);

				for (int j = 1; j <= 2; j++)
					lua_rawgeti(L, -j, j);

				coordinates[i * 2 + 0] = luaL_checknumber(L, -2);
				coordinates[i * 2 + 1] = luaL_checknumber(L, -1);

				lua_pop(L, 3);
			}
		}
	}

	for (int i = 0; i < args; i++)
	{
		if (currentScreen == renderScreen)
			graphicsPoints(coordinates[i * 2 + 0], coordinates[i * 2 + 1]);
	}

	delete[] coordinates;

	return 0;
}

int Graphics::Print(lua_State * L)
{
	const char * text = luaL_checkstring(L, 1);
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);

	Font * currFont = graphicsGetFont();

	if (currFont == nullptr)
		return 0;

	if (currentScreen == renderScreen)
	{
		C3D_Tex * texture = currFont->GetSheet()->GetTexture();

		bindTexture(texture);
		for (int i = 0; i < strlen(text); i++)
		{
			love::Glyph glyph = currFont->GetGlyph(text[i]);

			love::Quad * quad = glyph.GetQuad();

			graphicsDrawQuad(texture, x, y, quad->GetX(), quad->GetY(), quad->GetWidth(), quad->GetHeight());
		}
	}

	return 0;
}

int Graphics::SetFont(lua_State * L)
{
	Font * font = (Font *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_FONT);

	graphicsSetFont(font);
	
	return 0;
}

gfxScreen_t Graphics::GetScreen()
{
	return currentScreen;
}

void Graphics::InitRenderTargets()
{	
	this->topTarget = new CRenderTarget(GFX_TOP, GFX_LEFT, 400, 240);
	this->bottomTarget = new CRenderTarget(GFX_BOTTOM, GFX_LEFT, 320, 240);

	// shader and company
	dvlb = DVLB_ParseFile((u32 *)basic_shader_shbin, basic_shader_shbin_size);
	shaderProgramInit(&shader);
	shaderProgramSetVsh(&shader, &dvlb->DVLE[0]);
	C3D_BindProgram(&shader);

	C3D_CullFace(GPU_CULL_NONE);
	C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);
	
	projection_desc = shaderInstanceGetUniformLocation(shader.vertexShader, "projection");

	// set up mode defaults
	C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ONE, GPU_ONE_MINUS_SRC_ALPHA, GPU_ONE, GPU_ONE_MINUS_SRC_ALPHA); // premult

	graphicsInitWrapper();
}

void Graphics::Render(gfxScreen_t screen)
{
	resetPool();
	
	renderScreen = screen;
	
	C3D_FrameBegin(0); //SYNC_DRAW

	switch(screen)
	{
		case GFX_TOP:
			this->StartTarget(topTarget);
			break;
		case GFX_BOTTOM:
			this->StartTarget(bottomTarget);
			break;
	}
}

void Graphics::StartTarget(CRenderTarget * target)
{
	if (target->GetTarget() == nullptr)
		return;

	
	target->Clear(backgroundColor);
	
	C3D_FrameDrawOn(target->GetTarget());

	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, projection_desc, target->GetProjection());
}

void Graphics::SwapBuffers()
{
	C3D_FrameEnd(0);
}

int graphicsInit(lua_State * L)
{	
	gfxInitDefault();
	gfxSet3D(false);

	C3D_Init(0x80000 * 8);

	love::Graphics::Instance()->InitRenderTargets();
	
	luaL_Reg reg[] = 
	{
		{ "newImage",	imageNew					},
		{ "newFont",	fontNew						},
		{ "newQuad",	quadNew						},
		{ "line",		love::Graphics::Line		},
		{ "rectangle",	love::Graphics::Rectangle	},
		{ "circle",		love::Graphics::Circle		},
		{ "points",		love::Graphics::Points		},
		{ "draw",		love::Graphics::Draw		},
		{ "print",		love::Graphics::Print		},
		{ "setFont",	love::Graphics::SetFont		},
		{ "getWidth",	love::Graphics::GetWidth	},
		{ "getHeight",	love::Graphics::GetHeight	},
		{ "setScreen",	love::Graphics::SetScreen	},
		{ "setColor",	love::Graphics::SetColor	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}