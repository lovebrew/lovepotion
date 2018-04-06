#include "common/runtime.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "modules/graphics.h"
#include "modules/window.h"

#include "objects/image/image.h"
#include "objects/image/wrap_image.h"

#include "objects/quad/quad.h"
#include "objects/quad/wrap_quad.h"

#include "objects/font/glyph.h"
#include "objects/font/font.h"
#include "objects/font/wrap_font.h"

bool isInitialized = false;
u32 * FRAMEBUFFER;

SDL_Color backgroundColor = { 0, 0, 0, 255 };
SDL_Color drawColor = { 0, 0, 0, 255 };

Font * currentFont;

#define RGBA(r, g, b, a) r | (g >> (u32)8) | (b >> (u32)16) | (a >> (u32)24)

vector<StackMatrix> stack;
void transformDrawable(double * originalX, double * originalY) // rotate, scale, and translate coords.
{
	if (stack.empty())
		return;
		
	StackMatrix & matrix = stack.back();

	double newLeft = *originalX;
	double newTop = *originalY;

	//Translate
	*originalX += matrix.ox;
	*originalY += matrix.oy;

	//Scale
	*originalX *= matrix.sx;
	*originalY *= matrix.sy;

	//Rotate
	*originalX = newLeft * cos(matrix.r) - newTop * sin(matrix.r);
	*originalY = newLeft * sin(matrix.r) + newTop * cos(matrix.r);

	//Shear
	*originalX = newLeft + matrix.kx * newTop;
	*originalY = newTop + matrix.ky * newLeft;
}

void Graphics::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw Exception("Failed to load SDL2");

	TTF_Init();

	//stack.reserve(16);
	//stack.push_back(StackMatrix());

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

//love.graphics.setBackgroundColor
int Graphics::SetBackgroundColor(lua_State * L)
{
	double r = clamp(0, luaL_optnumber(L, 1, 0), 1);
	double g = clamp(0, luaL_optnumber(L, 2, 0), 1);
	double b = clamp(0, luaL_optnumber(L, 3, 0), 1);

	u8 outR, outG, outB;

	outR = r * 255;
	outG = g * 255;
	outB = b * 255;

	backgroundColor.r = outR;
	backgroundColor.g = outG;
	backgroundColor.b = outG;

	return 0;
}

//love.graphics.setColor
int Graphics::SetColor(lua_State * L)
{
	double r = clamp(0, luaL_optnumber(L, 1, 0), 1);
	double g = clamp(0, luaL_optnumber(L, 2, 0), 1);
	double b = clamp(0, luaL_optnumber(L, 3, 0), 1);
	double a = clamp(0, luaL_optnumber(L, 4, 1), 1);

	drawColor.r = r * 255;
	drawColor.g = g * 255;
	drawColor.b = b * 255;
	drawColor.a = a * 255;

	SDL_SetRenderDrawColor(Window::GetRenderer(), drawColor.r, drawColor.g, drawColor.b, drawColor.a);
}

//love.graphics.getBackgroundColor
int Graphics::GetBackgroundColor(lua_State * L)
{
	lua_pushnumber(L, backgroundColor.r / 255);
	lua_pushnumber(L, backgroundColor.g / 255);
	lua_pushnumber(L, backgroundColor.b / 255);

	return 3;
}

//love.graphics.clear
int Graphics::Clear(lua_State * L)
{
	SDL_SetRenderDrawColor(Window::GetRenderer(), backgroundColor.r, backgroundColor.g, backgroundColor.b, 255);
	SDL_RenderClear(Window::GetRenderer());

	return 0;
}

//love.graphics.present
int Graphics::Present(lua_State * L)
{
	SDL_RenderPresent(Window::GetRenderer());
	
	return 0;
}

//love.graphics.draw
int Graphics::Draw(lua_State * L)
{
	Image * graphic = (Image *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGE);
	Quad * quad = nullptr;

	int start = 2;
	if (!lua_isnoneornil(L, 2) && !lua_isnumber(L, 2))
	{
		quad = (Quad *)luaobj_checkudata(L, 2, LUAOBJ_TYPE_QUAD);
		start = 3;
	}

	double x = luaL_optnumber(L, start + 0, 0);
	double y = luaL_optnumber(L, start + 1, 0);

	double rotation = luaL_optnumber(L, start + 2, 0);

	double scalarX = luaL_optnumber(L, start + 3, 1);
	double scalarY = luaL_optnumber(L, start + 4, 1);

	SDL_Point point = {0, 0};

	SDL_Texture * texture = graphic->GetImage();
	
	SDL_Rect positionRectangle;
	SDL_Rect quadRectangle;

	SDL_RendererFlip flipHorizontal = SDL_FLIP_NONE;
	if (scalarX < 0.0)
		flipHorizontal = SDL_FLIP_HORIZONTAL;

	SDL_RendererFlip flipVertical = SDL_FLIP_NONE;
	if (scalarY < 0.0)
		flipVertical = SDL_FLIP_VERTICAL;

	transformDrawable(&x, &y);

	if (quad == nullptr)
	{
		quadRectangle = {0, 0, graphic->GetWidth(), graphic->GetHeight()};
		positionRectangle = {x, y, graphic->GetWidth(), graphic->GetHeight()};
	}
	else
	{
		quadRectangle = {quad->GetX(), quad->GetY(), quad->GetWidth(), quad->GetHeight()};
		positionRectangle = {x, y, quad->GetWidth(), quad->GetHeight()};
	}
	
	SDL_RenderCopyEx(Window::GetRenderer(), texture, &quadRectangle, &positionRectangle, rotation, &point, flipHorizontal);

	return 0;
}

//love.graphics.setFont
int Graphics::SetFont(lua_State * L)
{
	Font * self = (Font *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_FONT);

	if (self != NULL)
		currentFont = self;

	return 0;
}

//love.graphics.print
int Graphics::Print(lua_State * L)
{
	size_t length;
	const char * text = luaL_checklstring(L, 1, &length);
	
	double x = luaL_optnumber(L, 2, 0);
	double y = luaL_optnumber(L, 3, 0);
	
	transformDrawable(&x, &y);

	SDL_Rect position;
	position.x = x;
	position.y = y;

	SDL_Surface * source = TTF_RenderText_Blended(currentFont->GetFont(), text, drawColor);
	SDL_BlitSurface(source, NULL, Window::GetSurface(), &position);

	return 0;
}

//love.graphics.rectangle
int Graphics::Rectangle(lua_State * L)
{
	string mode = (string)luaL_checkstring(L, 1);

	double x = luaL_optnumber(L, 2, 0);
	double y = luaL_optnumber(L, 3, 0);

	double width = luaL_checknumber(L, 4);
	double height = luaL_checknumber(L, 5);

	transformDrawable(&x, &y);

	SDL_Rect rectangle = {x, y, width, height};
	
	if (mode == "fill")
		SDL_RenderFillRect(Window::GetRenderer(), &rectangle);
	else if (mode == "line")
		SDL_RenderDrawRect(Window::GetRenderer(), &rectangle);

}

//love.graphics.push
int Graphics::Push(lua_State * L)
{
	stack.push_back(stack.back());

	return 0;
}

//love.graphics.translate
int Graphics::Translate(lua_State * L)
{
	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);

	stack.back().ox += x;
	stack.back().oy += y;

	return 0;
}

//love.graphics.scale
int Graphics::Scale(lua_State * L)
{
	double sx = luaL_checknumber(L, 1);
	double sy = luaL_checknumber(L, 2);

	stack.back().sx = sx;
	stack.back().sy = sy;

	return 0;
}

//love.graphics.rotate
int Graphics::Rotate(lua_State * L)
{
	double r = luaL_checknumber(L, 1);

	stack.back().r = r;

	return 0;
}

//love.graphics.shear
int Graphics::Shear(lua_State * L)
{
	double kx = luaL_checknumber(L, 1);
	double ky = luaL_checknumber(L, 2);

	stack.back().kx = kx;
	stack.back().ky = ky;

	return 0;
}

//love.graphics.origin
int Graphics::Origin(lua_State * L)
{
	stack.back().ox = 0;
	stack.back().oy = 0;

	return 0;
}

//love.graphics.pop
int Graphics::Pop(lua_State * L)
{
	stack.pop_back();

	return 0;
}

//love.graphics.getRendererInfo
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
	printf("Qutting TTF\n");
	TTF_Quit();

	printf("Quitting Subsys: Video\n");
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	//SDL_Quit();
}

int Graphics::Register(lua_State * L)
{
	luaL_Reg reg[] =
	{
		{ "newFont",			fontNew				},
		{ "newImage",			imageNew			},
		{ "newQuad",			quadNew				},
		{ "draw",				Draw				},
		{ "print",				Print				},
		{ "setFont",			SetFont				},
		{ "rectangle",			Rectangle			},
		{ "clear",				Clear				},
		{ "push",				Push				},
		{ "translate",			Translate			},
		{ "scale",				Scale				},
		{ "rotate",				Rotate				},
		{ "origin",				Origin				},
		{ "shear",				Shear				},
		{ "present",			Present				},
		{ "setColor",			SetColor			},
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
