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

Color backgroundColor = { 0, 0, 0, 255 };
Color drawColor = { 0, 0, 0, 255 };

FT_Library FREETYPE_LIBRARY;
Font * currentFont;

#define RGBA(r, g, b, a) r | (g >> (u32)8) | (b >> (u32)16) | (a >> (u32)24)

void Graphics::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
		throw Exception("Failed to load SDL2");

	int error = FT_Init_FreeType(&FREETYPE_LIBRARY);
	if (error)
		throw Exception("Failed to load FreeType library.");

	isInitialized = true;
}

void renderText(u32 * target, float x, float y, FT_Bitmap * bitmap) 
{
	u32 screenwidth, screenheight;
	gfxGetFramebuffer((u32*)&screenwidth, (u32*)&screenheight);

	FT_UInt ix, iy;
	u32 pos;
	u32 color;

	for (iy = 0; iy < bitmap->rows; iy++)
	{
		for (ix = 0; ix < bitmap->width; ix++)
		{
			pos = (iy + y) * screenwidth + x + ix;
			color = bitmap->buffer[iy * bitmap->width + ix];
	
			//renderto(FRAMEBUFFER, pos, color, color, color, 255); //it's all just gonna be split to RGBA anyways
		} 
	}
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

int Graphics::GetBackgroundColor(lua_State * L)
{
	lua_pushnumber(L, backgroundColor.r / 255);
	lua_pushnumber(L, backgroundColor.g / 255);
	lua_pushnumber(L, backgroundColor.b / 255);

	return 3;
}

int Graphics::Clear(lua_State * L)
{
	SDL_SetRenderDrawColor(Window::GetRenderer(), backgroundColor.r, backgroundColor.g, backgroundColor.b, 255);
	SDL_RenderClear(Window::GetRenderer());

	return 0;
}

int Graphics::Present(lua_State * L)
{
	SDL_RenderPresent(Window::GetRenderer());
	
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

	/*vector<u8> data = graphic->GetImage();
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
	}*/

	return 0;
}

int Graphics::SetFont(lua_State * L)
{
	Font * self = (Font *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_FONT);

	if (self != NULL)
		currentFont = self;

	return 0;
}

int Graphics::Print(lua_State * L)
{
	size_t length;
	const char * text = luaL_checklstring(L, 1, &length);
	
	float x = luaL_optnumber(L, 2, 0);
	float y = luaL_optnumber(L, 3, 0);

	int error;

	int pen_x = x;
	int pen_y = y;

	int advanceX = 0;
	int advanceY = 0;

	int offsetX = 0;
	int offsetY = 0;

	for (int i = 0; i < length; i++)
	{
		FT_Face face = currentFont->GetFace();

		if (face)
		{
			if (text[i] == '\n')
			{
				pen_y += currentFont->GetSize();
				x = pen_x;
			}
			else
			{
				FT_UInt  glyph_index;
				
				/* retrieve glyph index from character code */
				glyph_index = FT_Get_Char_Index(face, text[i]);

				/* load glyph image into the slot (erase previous one) */
				error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
				if ( error )
					continue;  /* ignore errors */

				/* convert to an anti-aliased bitmap */
				error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
				if ( error )
					continue;

				//offsetX = currentFont->GetGlyphData(glyph_index, face, "cornerX");
				//offsetY = currentFont->GetGlyphData(glyph_index, face, "cornerY");

				renderText(FRAMEBUFFER, x + face->glyph->bitmap_left, pen_y - face->glyph->bitmap_top, &face->glyph->bitmap);
				
				x += face->glyph->advance.x >> 6;
				pen_y += face->glyph->advance.y >> 6;
			}
		}
	}

	return 0;
}

int Graphics::Rectangle(lua_State * L)
{
	string mode = (string)luaL_checkstring(L, 1);

	double x = luaL_optnumber(L, 2, 0);
	double y = luaL_optnumber(L, 3, 0);

	double width = luaL_checknumber(L, 4);
	double height = luaL_checknumber(L, 5);

	SDL_Rect rectangle = {x, y, width, height};
	
	if (mode == "fill")
		SDL_RenderFillRect(Window::GetRenderer(), &rectangle);
	else if (mode == "line")
		SDL_RenderDrawRect(Window::GetRenderer(), &rectangle);

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

FT_Library Graphics::GetFreetypeLibrary()
{
	return FREETYPE_LIBRARY;
}

bool Graphics::IsInitialized()
{
	return isInitialized;
}

void Graphics::Exit()
{
	FT_Done_FreeType(FREETYPE_LIBRARY);

	gfxExit();
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
