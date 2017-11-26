#include "common/runtime.h"
#include "crendertarget.h"

#include "graphics.h"
#include "imagedata.h"
#include "image.h"
#include "file.h"
#include "quad.h"
#include "glyph.h"
#include "font.h"

#include "wrap_image.h"
#include "wrap_font.h"
#include "wrap_quad.h"
#include "wrap_graphics.h"

#include "canvas.h"
#include "wrap_canvas.h"

#include "basic_shader_shbin.h"

love::Graphics * love::Graphics::instance = 0;

using love::Graphics;
using love::CRenderTarget;
using love::Image;
using love::Font;
using love::Quad;
using love::Canvas;

int projection_desc = -1;

DVLB_s * dvlb = nullptr;
shaderProgram_s shader;

gfxScreen_t currentScreen = GFX_TOP;
gfxScreen_t renderScreen = GFX_TOP;
gfx3dSide_t currentSide = GFX_LEFT;

Canvas * lastCanvas = nullptr;

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
	int r = graphicsGetColor(0);
	int g = graphicsGetColor(1);
	int b = graphicsGetColor(2);
	int a = graphicsGetColor(3);

	if (!lua_istable(L, 1))
	{
		r = luaL_checkinteger(L, 1);
		g = luaL_checkinteger(L, 2);
		b = luaL_checkinteger(L, 3);
		a = luaL_optnumber(L, 4, a);
	}
	else
	{
		for (int i = 1; i <= 4; i++)
			lua_rawgeti(L, 1, i);
		
		r = luaL_checkinteger(L, -4);
		g = luaL_checkinteger(L, -3);
		b = luaL_checkinteger(L, -2);
		a = luaL_optnumber(L, -1, a);
	}

	graphicsSetColor(r, g, b, a);

	return 0;
}

int Graphics::SetBackgroundColor(lua_State * L)
{
	int r, g, b;
	if (!lua_istable(L, 1))
	{
		r = luaL_checknumber(L, 1);
		g = luaL_checknumber(L, 2);
		b = luaL_checknumber(L, 3);
	}
	else
	{
		for (int i = 1; i <= 4; i++)
			lua_rawgeti(L, 1, i);
			
		r = luaL_checkinteger(L, -4);
		g = luaL_checkinteger(L, -3);
		b = luaL_checkinteger(L, -2);
	}

	graphicsSetBackgroundColor(r, g, b);

	return 0;
}

int Graphics::GetBackgroundColor(lua_State * L)
{
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
	{
		if (strncmp(mode, "fill", 4) == 0)
			graphicsRectangle(x, y, width, height);
		else if (strncmp(mode, "line", 4) == 0)
		{
			graphicsLine(x, y, x + width, y); //top-left->top-right
			graphicsLine(x, y, x, y + height); //top-left->bottom-left
			graphicsLine(x, y + height, x + width, y + height); //bottom-left->bottom->right
			graphicsLine(x + width, y, x + width, y + height);
		}
	}

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
	{
		if (strncmp(mode, "fill", 4) == 0)
			graphicsCircle(x, y, radius, segments);
		else
		{
			float angle = 0;
			for (int i = 0; i <= segments; i++)
			{
				graphicsCircle(x + cos(angle) * radius, y + sin(angle) * radius, 1, segments);
				angle = angle + 2 * M_PI / segments;
			}
		}
	}	

	return 0;
}

int Graphics::Draw(lua_State * L)
{
	Image * image = (Image *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGE);
	Canvas * canvas = nullptr;
	Quad * quad = nullptr;

	if (image == NULL)
	{
		canvas = (Canvas *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_CANVAS);

		if (canvas->GetTexture() != nullptr)
			image = canvas->GetTexture();
	}
	
	int start = 2;
	if (!lua_isnoneornil(L, 2) && !lua_isnumber(L, 2))
	{
		quad = (Quad *)luaobj_checkudata(L, 2, LUAOBJ_TYPE_QUAD);
		start = 3;
	}

	float x = luaL_optnumber(L, start, 0);
	float y = luaL_optnumber(L, start + 1, 0);

	float rotation = luaL_optnumber(L, start + 2, 0);

	float scalarX = luaL_optnumber(L, start + 3, 1);
	float scalarY = luaL_optnumber(L, start + 4, 1);

	float offsetX = luaL_optnumber(L, start + 5, 0);
	float offsetY = luaL_optnumber(L, start + 6, 0);

	if (image != NULL)
	{
		if (currentScreen == renderScreen)
		{
			bindTexture(image->GetTexture());
			if (quad == nullptr)
				graphicsDraw(image->GetTexture(), x, y, image->GetWidth(), image->GetHeight(), rotation, scalarX, scalarY, offsetX, offsetY);
			else
				graphicsDrawQuad(image->GetTexture(), x, y, quad->GetX(), quad->GetY(), quad->GetWidth(), quad->GetHeight(), rotation, scalarX, scalarY, offsetX, offsetY);
		}
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
	float x = luaL_optnumber(L, 2, 0);
	float y = luaL_optnumber(L, 3, 0);

	if (currentScreen == renderScreen)
		graphicsPrint(text, x, y);
	
	return 0;
}

int Graphics::Printf(lua_State * L)
{
	const char * text = luaL_checkstring(L, 1);
	float x = luaL_optnumber(L, 2, 0);
	float y = luaL_optnumber(L, 3, 0);
	float limit = luaL_optnumber(L, 4, -1);

	if (currentScreen == renderScreen)
		graphicsPrintf(text, x, y, limit);
	
	return 0;
}

int Graphics::SetFont(lua_State * L)
{
	Font * font = (Font *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_FONT);

	if (graphicsGetFont() != font)
		graphicsSetFont(font);
	
	return 0;
}

int Graphics::GetFont(lua_State * L) //Doesn't work yet
{
	/*Font * currentFont = graphicsGetFont();

	lua_pushlightuserdata(L, currentFont);
	lua_gettable(L, -1);
	lua_remove(L, -1);

	return 1;*/
	return 0;
}

int Graphics::Set3D(lua_State * L)
{
	bool enable = lua_toboolean(L, 1);

	graphicsSet3D(enable);

	return 0;
}

int Graphics::SetDepth(lua_State * L)
{
	float depth = luaL_checknumber(L, 1);

	graphicsSetDepth(depth);

	return 0;
}

int Graphics::Push(lua_State * L)
{
	graphicsPush();

	return 0;
}

int Graphics::Pop(lua_State * L)
{
	graphicsPop();

	return 0;
}

int Graphics::Translate(lua_State * L)
{
	float translateX = luaL_checknumber(L, 1);
	float translateY = luaL_checknumber(L, 2);

	graphicsTranslate(translateX, translateY);

	return 0;
}

int Graphics::SetScissor(lua_State * L)
{
	int args = lua_gettop(L);

	float x = luaL_optnumber(L, 1, 0);
	float y = luaL_optnumber(L, 2, 0);
	float width = luaL_optnumber(L, 3, 0);
	float height = luaL_optnumber(L, 4, 0);

	int screenWidth = 400;
	if (currentScreen == GFX_BOTTOM)
		screenWidth = 320;

	if (currentScreen == renderScreen)
		graphicsSetScissor(args == 0, 240 - (y + height), screenWidth - (x + width), 240 - y, screenWidth - x);

	return 0;
}

int Graphics::SetDefaultFilter(lua_State * L)
{
	const char * min = luaL_checkstring(L, 1);
	const char * mag = luaL_checkstring(L, 2);

	graphicsSetDefaultFilter(min, mag);

	return 0;
}

int Graphics::Clear(lua_State * L)
{
	u32 backgroundColor = graphicsGetBackgroundColor();

	if (lastCanvas != nullptr)
		lastCanvas->Clear(backgroundColor);

	love::Graphics::Instance()->topDepthTarget->Clear(backgroundColor);
	love::Graphics::Instance()->topTarget->Clear(backgroundColor);
	love::Graphics::Instance()->bottomTarget->Clear(backgroundColor);

	return 0;
}

int Graphics::GetRendererInfo(lua_State * L)
{
	lua_pushstring(L, "OpenGL ES");
	lua_pushstring(L, "1.1");
	lua_pushstring(L, "Digital Media Professionals Inc.");
	lua_pushstring(L, "DMP PICA200");
 
	return 4;
}

int Graphics::Present(lua_State * L)
{
	love::Graphics::Instance()->SwapBuffers();

	return 0;
}

//Graphics.cpp
int Graphics::SetCanvas(lua_State * L)
{
	if (!lua_isnoneornil(L, 1))
	{	
		Canvas * self = (Canvas *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_CANVAS);

		if (self->GetTarget()->GetTarget() == nullptr)
			return 0;

		love::Graphics::Instance()->StartTarget(self->GetTarget());

		lastCanvas = self;
	}

	return 0;
}

gfxScreen_t Graphics::GetScreen()
{
	return currentScreen;
}

gfx3dSide_t Graphics::GetSide()
{
	return currentSide;
}

void Graphics::InitRenderTargets()
{	
	this->topTarget = new CRenderTarget(GFX_TOP, GFX_LEFT, 400, 240);
	this->bottomTarget = new CRenderTarget(GFX_BOTTOM, GFX_LEFT, 320, 240);
	this->topDepthTarget = new CRenderTarget(GFX_TOP, GFX_RIGHT, 400, 240);

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

void Graphics::Render(gfxScreen_t screen, gfx3dSide_t side)
{
	renderScreen = screen;

	switch(screen)
	{
		case GFX_TOP:
			if (side == GFX_LEFT)
				this->StartTarget(topTarget);
			else
				this->StartTarget(topDepthTarget);
			break;
		case GFX_BOTTOM:
			this->StartTarget(bottomTarget);
			break;
	}

	currentSide = side;
}

void Graphics::EnsureInRender()
{
	if (!this->inRender)
	{
		resetPool();
			
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		this->inRender = true;
	}
}

void Graphics::StartTarget(CRenderTarget * target)
{
	this->EnsureInRender();

	C3D_FrameDrawOn(target->GetTarget());
	
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, projection_desc, target->GetProjection());
}

void Graphics::Clear(gfxScreen_t screen, gfx3dSide_t side)
{
	CRenderTarget * target = nullptr;

	switch(screen)
	{
		case GFX_TOP:
			if (side == GFX_LEFT)
				target = topTarget;
			else
				target = topDepthTarget;
			break;
		case GFX_BOTTOM:
			target = bottomTarget;
			break;
	}

	target->Clear(graphicsGetBackgroundColor());
}

void Graphics::SwapBuffers()
{
	if (!this->inRender)
		return;

	this->inRender = false;

	C3D_FrameEnd(0);
}

bool Graphics::InFrame()
{
	return this->inRender;
}

void graphicsExit()
{
	C3D_Fini();
	gfxExit();
}

int graphicsInit(lua_State * L)
{	
	gfxInitDefault();
	gfxSet3D(false);

	//resetPool();
	C3D_Init(0x80000 * 8);

	love::Graphics::Instance()->InitRenderTargets();
	
	luaL_Reg reg[] = 
	{
		{ "newImage",			imageNew					},
		{ "newFont",			fontNew						},
		{ "newQuad",			quadNew						},
		{ "newCanvas",			canvasNew					},
		{ "line",				love::Graphics::Line		},
		{ "rectangle",			love::Graphics::Rectangle	},
		{ "circle",				love::Graphics::Circle		},
		{ "points",				love::Graphics::Points		},
		{ "draw",				love::Graphics::Draw		},
		{ "print",				love::Graphics::Print		},
		{ "printf",				love::Graphics::Printf		},
		{ "setFont",			love::Graphics::SetFont		},
		{ "getFont",			love::Graphics::GetFont		},
		{ "getWidth",			love::Graphics::GetWidth	},
		{ "getHeight",			love::Graphics::GetHeight	},
		{ "setScreen",			love::Graphics::SetScreen	},
		{ "setColor",			love::Graphics::SetColor	},
		{ "setBackgroundColor",	love::Graphics::SetBackgroundColor	},
		{ "getBackgroundColor", love::Graphics::GetBackgroundColor	},
		{ "getRendererInfo", 	love::Graphics::GetRendererInfo		},
		{ "set3D",				love::Graphics::Set3D		},
		{ "setDepth",			love::Graphics::SetDepth	},
		{ "push",				love::Graphics::Push		},
		{ "pop",				love::Graphics::Pop			},
		{ "translate",			love::Graphics::Translate	},
		{ "setScissor",			love::Graphics::SetScissor	},
		{ "setDefaultFilter",	love::Graphics::SetDefaultFilter},
		{ "clear",				love::Graphics::Clear		},
		{ "present",			love::Graphics::Present		},
		{ "setCanvas",			love::Graphics::SetCanvas	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}