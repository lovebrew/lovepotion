#include "common/runtime.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "modules/graphics.h"
#include "modules/window.h"

#include "common/drawable.h"
#include "objects/image/image.h"
#include "objects/image/wrap_image.h"

#include "objects/quad/quad.h"
#include "objects/quad/wrap_quad.h"

#include "objects/font/font.h"
#include "objects/font/wrap_font.h"

#include "objects/canvas/canvas.h"
#include "objects/canvas/wrap_canvas.h"

bool isInitialized = false;
u32 * FRAMEBUFFER;

SDL_Color backgroundColor = { 0, 0, 0, 255 };
SDL_Color drawColor = { 255, 255, 255, 255 };

Font * currentFont;
Canvas * currentCanvas;

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

	stack.reserve(16);
	stack.push_back(StackMatrix());

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
	double r = 0, g = 0, b = 0;

	if (lua_isnumber(L, 1))
	{
		r = clamp(0, luaL_optnumber(L, 1, 0), 1);
		g = clamp(0, luaL_optnumber(L, 2, 0), 1);
		b = clamp(0, luaL_optnumber(L, 3, 0), 1);
	}
	else if (lua_istable(L, 1))
	{
		for (int i = 1; i <= 4; i++)
			lua_rawgeti(L, 1, i);

		r = clamp(0, luaL_optnumber(L, -4, 0), 1);
		g = clamp(0, luaL_optnumber(L, -3, 0), 1);
		b = clamp(0, luaL_optnumber(L, -2, 0), 1);
	}

	backgroundColor.r = r * 255;
	backgroundColor.g = g * 255;
	backgroundColor.b = b * 255;

	return 0;
}

//love.graphics.setColor
int Graphics::SetColor(lua_State * L)
{
	double r = 0, g = 0, b = 0, a = 1;

	if (lua_isnumber(L, 1))
	{
		r = clamp(0, luaL_optnumber(L, 1, 0), 1);
		g = clamp(0, luaL_optnumber(L, 2, 0), 1);
		b = clamp(0, luaL_optnumber(L, 3, 0), 1);
		a = clamp(0, luaL_optnumber(L, 4, 1), 1);
	}
	else if (lua_istable(L, 1))
	{
		for (int i = 1; i <= 4; i++)
			lua_rawgeti(L, 1, i);

		r = clamp(0, luaL_optnumber(L, -4, 0), 1);
		g = clamp(0, luaL_optnumber(L, -3, 0), 1);
		b = clamp(0, luaL_optnumber(L, -2, 0), 1);
		a = clamp(0, luaL_optnumber(L, -1, 1), 1);
	}

	drawColor.r = r * 255;
	drawColor.g = g * 255;
	drawColor.b = b * 255;
	drawColor.a = a * 255;

	SDL_SetRenderDrawColor(Window::GetRenderer(), drawColor.r, drawColor.g, drawColor.b, drawColor.a);

	return 0;
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

	SDL_SetRenderDrawColor(Window::GetRenderer(), drawColor.r, drawColor.g, drawColor.b, drawColor.a);

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
	Drawable * drawable = (Image *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGE);
	
	/*
	** Check if it is NULL or not
	** If it is, check if it's a canvas
	** Elsewise, .. crash
	*/
	if (drawable == NULL)
	{
		drawable = (Canvas *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_CANVAS);

		if (drawable == NULL)
			return luaL_error(L, "Drawable expected, got %s", lua_tostring(L, 1));
	}

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
	
	SDL_Rect positionRectangle;
	SDL_Rect quadRectangle;

	SDL_RendererFlip flipHorizontal = SDL_FLIP_NONE;
	if (scalarX < 0.0)
		flipHorizontal = SDL_FLIP_HORIZONTAL;

	SDL_RendererFlip flipVertical = SDL_FLIP_NONE;
	if (scalarY < 0.0)
		flipVertical = SDL_FLIP_VERTICAL;

	transformDrawable(&x, &y);
	
	if (quad != nullptr)
	{
		quadRectangle = {quad->GetX(), quad->GetY(), quad->GetWidth(), quad->GetHeight()};
		drawable->Draw(&quadRectangle, x, y, rotation, 1, 1);
	}
	else
		drawable->Draw(NULL, x, y, rotation, 1, 1);

	return 0;
}

//love.graphics.getFont
int Graphics::GetFont(lua_State * L)
{
	/*lua_pushlightuserdata(L, currentFont);
	lua_pushvalue(L, -1);
	*/
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

//love.graphics.setNewFont
int Graphics::SetNewFont(lua_State * L)
{
	int ret = fontNew(L);
	Font * self = (Font *)luaobj_checkudata(L, -1, LUAOBJ_TYPE_FONT);
	currentFont = self;

	return ret;
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

	SDL_Surface * source = TTF_RenderText_Blended_Wrapped(currentFont->GetFont(), text, drawColor, 1280);
	SDL_BlitSurface(source, NULL, Window::GetSurface(), &position);

	return 0;
}

//love.graphics.setCanvas
int Graphics::SetCanvas(lua_State * L)
{
	Canvas * self = NULL;
	if (!lua_isnoneornil(L, 1))
		self = (Canvas *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_CANVAS);

	if (self != NULL)
	{
		self->SetAsTarget();
		Graphics::Clear(L);
	}
	else
		SDL_SetRenderTarget(Window::GetRenderer(), NULL);

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

	return 0;
}

//love.graphics.line
int Graphics::Line(lua_State * L)
{
	int argc = lua_gettop(L);
	bool isTable = lua_istable(L, 1);
	int tableLength = 0;

	double startx = 0, starty = 0, endx = 0, endy = 0;

	if (isTable)
	{
		tableLength = lua_objlen(L, 1);

		if (tableLength == 0 || (tableLength > 0 && (tableLength % 4) != 0))
			return luaL_error(L, "%s", "Need at least two verticies to draw a line");

		if ((tableLength % 4) == 0)
		{
			for (int i = 1; i < tableLength; i += 4)
				lua_rawgeti(L, 1, i);

			startx = luaL_checknumber(L, -4);
			starty = luaL_checknumber(L, -3);
			
			endx = luaL_checknumber(L, -2);
			endy = luaL_checknumber(L, -1);

			lua_pop(L, 4);

			SDL_RenderDrawLine(Window::GetRenderer(), startx, starty, endx, endy);
		}
	}
	else
	{
		if ((argc % 4) != 0)
			return luaL_error(L, "%s", "Need at least two verticies to draw a line");
		
		for (int i = 0; i < argc; i += 4)
		{
			startx = luaL_checknumber(L, i + 1);
			starty = luaL_checknumber(L, i + 2);

			endx = luaL_checknumber(L, i + 3);
			endy = luaL_checknumber(L, i + 4);

			SDL_RenderDrawLine(Window::GetRenderer(), startx, starty, endx, endy);
		}
	}

	return 0;
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
	lua_pushstring(L, "Tegra X1");

	return 4;
}

//End Löve2D Functions

bool Graphics::IsInitialized()
{
	return isInitialized;
}

void Graphics::Exit()
{
	TTF_Quit();

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();
}

int Graphics::Register(lua_State * L)
{
	luaL_Reg reg[] =
	{
		{ "newFont",			fontNew				},
		{ "newImage",			imageNew			},
		{ "newCanvas",			canvasNew			},
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
		{ "setNewFont",			SetNewFont			},
		{ "setCanvas",			SetCanvas			},
		{ "getFont",			GetFont				},
		{ "line",				Line				},
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
