#include "runtime.h"
#include "graphics.h"

#include "image.h"
#include "wrap_image.h"

#include "quad.h"
#include "wrap_quad.h"

#include "glyph.h"
#include "font.h"
#include "wrap_font.h"

//Rendertargets
C3D_RenderTarget * topTarget = nullptr;
C3D_RenderTarget * depthTarget = nullptr;
C3D_RenderTarget * bottomTarget = nullptr;

//Colors
u32 backgroundColor = C2D_Color32(0, 0, 0, 1);

int currentAlpha = 255;
u32 graphicsColor = C2D_Color32(255, 255, 255, currentAlpha);

//Configure default image tint
C2D_ImageTint currentTint;
C2D_ImageTint fontTint;

//Make sure we can do stuff
bool isInitialized = false;

Font * currentFont = nullptr;

//Current screen
gfxScreen_t currentScreen = GFX_TOP;
gfx3dSide_t currentSide = GFX_LEFT;
gfxScreen_t renderScreen;

//for tranlating
bool graphicsPushed = false;
vector<vector<float>> transformstack; //  tfstck[0] -> 1 = translate, 2 = scale, 3 = rotate
float translateX = 0;
float translateY = 0;
float currentDepth = 0;

void transformDrawable(float * originalX, float * originalY, float * originalAngle) // rotate, scale, and translate coords.
{
	float newX = 0.0f;
	float newY = 0.0f;
	float slider = CONFIG_3D_SLIDERSTATE;

	for(int i = transformstack.size() - 1; i >= 0; i--)
	{
		switch(static_cast<int>(transformstack[i][0]))
		{
			case 1 : // translate
				*originalX += transformstack[i][1];
				*originalY += transformstack[i][2];
				
				break;
			case 2 : // scale
				*originalX *= transformstack[i][1];
				*originalY *= transformstack[i][2];

				break;
			case 3 : // rotate
				newX = *originalX; 
				newY = *originalY;

				*originalX = newX * cos(transformstack[i][1]) - newY * sin(transformstack[i][1]);
				*originalY = newX * sin(transformstack[i][1]) + newY * cos(transformstack[i][1]);

				*originalAngle = transformstack[i][1];

				break;
			case 4 : // shear
				newX = *originalX;
				newY = *originalY;

				*originalX = newX + transformstack[i][1] * newY;
				*originalY = newY + transformstack[i][2] * newX;

				break;
			default :
				Console::ThrowError("Invalid transformstack ID: %d", i);
		}
	}

	if (gfxIs3D() && currentScreen == GFX_TOP)
	{
		if (currentSide == GFX_LEFT)
			*originalX -= (slider * currentDepth);
		else if (currentSide == GFX_RIGHT)
			*originalX += (slider * currentDepth);
	}
}

void Graphics::Initialize()
{
	if (!Console::IsInitialized())
		gfxInitDefault();

	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	
	topTarget = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	bottomTarget = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	depthTarget = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
	
	C2D_PlainImageTint(&currentTint, graphicsColor, 0.0f);
	C2D_PlainImageTint(&fontTint, graphicsColor, 0.0f);

	isInitialized = true;
}

//Löve2D Functions

//love.graphics.setBackgroundColor
int Graphics::SetBackgroundColor(lua_State * L)
{
	int r, g, b;

	if (!lua_istable(L, 1))
	{
		r = luaL_checkinteger(L, 1);
		g = luaL_checkinteger(L, 2);
		b = luaL_checkinteger(L, 3);
	}
	else
	{
		for (int i = 1; i <= 4; i++)
			lua_rawgeti(L, 1, i);
		
		r = luaL_checkinteger(L, -4);
		g = luaL_checkinteger(L, -3);
		b = luaL_checkinteger(L, -2);
	}

	backgroundColor = C2D_Color32(r, g, b, 255);

	return 0;
}

//love.graphics.setColor
int Graphics::SetColor(lua_State * L)
{
	int r, g, b;
	int a = currentAlpha;

	if (!lua_istable(L, 1))
	{
		r = luaL_checkinteger(L, 1);
		g = luaL_checkinteger(L, 2);
		b = luaL_checkinteger(L, 3);
		currentAlpha = luaL_optnumber(L, 4, a);
	}
	else
	{
		for (int i = 1; i <= 4; i++)
			lua_rawgeti(L, 1, i);
		
		r = luaL_checkinteger(L, -4);
		g = luaL_checkinteger(L, -3);
		b = luaL_checkinteger(L, -2);
		currentAlpha = luaL_optnumber(L, -1, a);
	}

	float alpha = currentAlpha / 255.0f;
	graphicsColor = C2D_Color32(r * alpha, g * alpha, b * alpha, currentAlpha);

	C2D_AlphaImageTint(&currentTint, alpha);
	C2D_PlainImageTint(&fontTint, graphicsColor, 1);

	return 0;
}

//love.graphics.setScreen
int Graphics::SetScreen(lua_State * L)
{
	const char * screen = luaL_checkstring(L, 1);

	gfxScreen_t switchScreen = GFX_TOP;
	if (strncmp(screen, "bottom", 6) == 0)
		switchScreen = GFX_BOTTOM;

	currentScreen = switchScreen;

	return 0;
}

int Graphics::SetDefaultFilter(lua_State * L)
{
	return 0;
}

//love.graphics.getWidth
int Graphics::GetWidth(lua_State * L)
{
	int width = 400;
	if (currentScreen == GFX_BOTTOM)
		width = 320;
	
	lua_pushnumber(L, width);

	return 1;
}

//love.graphics.getHeight
int Graphics::GetHeight(lua_State * L)
{
	lua_pushnumber(L, 240);

	return 1;
}

//love.graphics.setFont
int Graphics::SetFont(lua_State * L)
{
	currentFont = (Font *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_FONT);

	return 0;
}

int Graphics::Rectangle(lua_State * L)
{
	const char * mode = luaL_checkstring(L, 1);
	float x = luaL_optnumber(L, 2, 0);
	float y = luaL_optnumber(L, 3, 0);
	float width = luaL_checknumber(L, 4);
	float height = luaL_checknumber(L, 5);

	transformDrawable(&x, &y, 0);
	if (currentScreen == renderScreen)
	{
		if (strncmp(mode, "fill", 4) == 0)
			C2D_DrawRectSolid(x, y, 0.5, width, height, graphicsColor);
		else if (strncmp(mode, "line", 4) == 0)
		{
			C2D_DrawRectSolid(x, y, 0.5, 1, height, graphicsColor); // tl -> bl
			C2D_DrawRectSolid(x + 1, y, 0.5, width - 1, 1, graphicsColor); // tl -> tr
			C2D_DrawRectSolid(x + width, y, 0.5, 1, height, graphicsColor); // tr -> br
			C2D_DrawRectSolid(x + 1, y + height, 0.5, width - 1, 1, graphicsColor); // bl -> br
		}
	}
	
	return 0;
}

int Graphics::Circle(lua_State * L)
{
	const char * mode = luaL_checkstring(L, 1);
	float x = luaL_optnumber(L, 2, 0);
	float y = luaL_optnumber(L, 3, 0);
	float radius = luaL_checknumber(L, 4);
	float segments = luaL_optnumber(L, 5, 100);

	transformDrawable(&x, &y, 0);
	if (currentScreen == renderScreen)
		C2D_DrawRectSolid(x - radius, y - radius, 0.5, radius * 2, radius * 2, graphicsColor);

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
	{
		if (width < 0 || height < 0)
			Console::ThrowError("Scissor cannot have negative width or height.");

		GPU_SCISSORMODE mode = GPU_SCISSOR_NORMAL;
		if (args == 0)
			mode = GPU_SCISSOR_DISABLE;

		C3D_SetScissor(mode, 240 - (y + height), screenWidth - (x + width), 240 - y, screenWidth - x);
	}

	return 0;
}

int Graphics::Print(lua_State * L)
{
	if (currentFont == nullptr)
		return 0;

	C2D_Image drawable = currentFont->GetImage();
	Tex3DS_SubTexture subtex;

	drawable.subtex = &subtex;

	size_t textLength;
	const char * text = luaL_checklstring(L, 1, &textLength);

	float x = luaL_optnumber(L, 2, 0);
	float y = luaL_optnumber(L, 3, 0);

	int width = 0;
	float originX = x;

	transformDrawable(&x, &y, 0);
	
	for (uint i = 0; i < textLength; i++)
	{
		char letter = text[i];

		if (text[i] == '\n')
		{
			x = originX;
			y = y + currentFont->GetHeight() + 2;
			width = 0;
		}
		else
		{
			if (!currentFont->HasGlyph(letter))
				return 0;

			Quad * glyph = currentFont->GetGlyph(letter);
			
			if (glyph == nullptr)
				Console::ThrowError("This shouldn't happen. If it does, panic.");

			glyph->SetSubTexture(&subtex);

			if (i > 0)
				width += currentFont->GetGlyphData(text[i - 1], 2);

			if (currentScreen == renderScreen)
				C2D_DrawImageAt(drawable, x + width + currentFont->GetGlyphData(letter, 0), y + currentFont->GetGlyphData(letter, 1), 0.5, &fontTint, 1, 1);
		}
	}

	return 0;
}

//love.graphics.draw
int Graphics::Draw(lua_State * L)
{
	Image * image = (Image *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGE);
	Quad * quad = nullptr;

	Tex3DS_SubTexture subtex;
	C2D_Image drawable = image->GetImage();

	int start = 2;
	if (!lua_isnoneornil(L, 2) && !lua_isnumber(L, 2))
	{
		quad = (Quad *)luaobj_checkudata(L, 2, LUAOBJ_TYPE_QUAD);

		quad->SetSubTexture(&subtex);
		drawable.subtex = &subtex;
	
		start = 3;
	}

	float x = luaL_optnumber(L, start + 0, 0);
	float y = luaL_optnumber(L, start + 1, 0);
	float rotation = luaL_optnumber(L, start + 2, 0);

	float scalarX = luaL_optnumber(L, start + 3, 1.0f);
	float scalarY = luaL_optnumber(L, start + 4, 1.0f);
	
	float offsetX = luaL_optnumber(L, start + 5, 0);
	float offsetY = luaL_optnumber(L, start + 6, 0);

	x += offsetX;
	y += offsetY;

	transformDrawable(&x, &y, &rotation);

	if (currentScreen == renderScreen)
	{
		if (rotation == 0)
			C2D_DrawImageAt(drawable, x, y, 0.5, &currentTint, scalarX, scalarY);
		else
			C2D_DrawImageAtRotated(drawable, x, y, 0.5f, rotation, &currentTint, scalarX, scalarY);
	}

	return 0;
}

int Graphics::Set3D(lua_State * L)
{
	bool enable = false;
	if (lua_isboolean(L, 1))
		enable = lua_toboolean(L, 1);

	gfxSet3D(enable);

	return 0;
}

int Graphics::SetDepth(lua_State * L)
{
	currentDepth = luaL_checknumber(L, 1);

	return 0;
}

//love.graphics.getRendererInfo
int Graphics::GetRendererInfo(lua_State * L)
{
	lua_pushstring(L, "OpenGL ES");
	lua_pushstring(L, "1.1");
	lua_pushstring(L, "Digital Media Professionals Inc.");
	lua_pushstring(L, "DMP PICA200");
 
	return 4;
}

//love.graphics.push
int Graphics::Push(lua_State * L)
{
	if (currentScreen == renderScreen)
		graphicsPushed = true;

	return 0;
}

//love.graphics.translate
int Graphics::Translate(lua_State * L)
{
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);

	if (currentScreen == renderScreen)
	{
		if (graphicsPushed)
			transformstack.push_back({1.0f, x, y});
	}

	return 0;
}

//love.graphics.scale
int Graphics::Scale(lua_State * L)
{
	float sx = luaL_checknumber(L, 1);
	float sy = luaL_checknumber(L, 2);

	if (currentScreen == renderScreen)
	{
		if (graphicsPushed)
			transformstack.push_back({2.0f, sx, sy});
	}

	return 0;
}

//love.graphics.rotate
int Graphics::Rotate(lua_State * L)
{
	float r = luaL_checknumber(L, 1);

	if (currentScreen == renderScreen)
	{
		if (graphicsPushed)
			transformstack.push_back({3.0f, r});
	}

	return 0;
}

//love.graphics.shear
int Graphics::Shear(lua_State * L)
{
	float kx = luaL_checknumber(L, 1);
	float ky = luaL_checknumber(L, 2);

	if (currentScreen == renderScreen)
	{
		if (graphicsPushed)
			transformstack.push_back({4.0f, kx, ky});
	}

	return 0;
}

//love.graphics.origin
int Graphics::Origin(lua_State * L)
{
	if (currentScreen == renderScreen)
	{
		if (graphicsPushed)
			transformstack.clear();
	}

	return 0;
}

//love.graphics.pop
int Graphics::Pop(lua_State * L)
{
	if (currentScreen == renderScreen)
	{
		graphicsPushed = false;
		transformstack.clear();
	}

	return 0;
}

//love.graphics.clear
int Graphics::Clear(lua_State * L)
{
	C2D_TargetClear(GetScreen(GFX_TOP, GFX_LEFT), backgroundColor);
	C2D_TargetClear(GetScreen(GFX_TOP, GFX_RIGHT), backgroundColor);
	C2D_TargetClear(GetScreen(GFX_BOTTOM, GFX_LEFT), backgroundColor);

	return 0;
}

//love.graphics.present
int Graphics::Present(lua_State * L)
{
	C3D_FrameEnd(0);

	return 0;
}

//End Löve2D Functions

bool Graphics::IsInitialized()
{
	return isInitialized;
}

C3D_RenderTarget * Graphics::GetScreen(gfxScreen_t screen, gfx3dSide_t side)
{
	C3D_RenderTarget * target = nullptr;

	switch(screen)
	{
		case GFX_TOP:
			if (side == GFX_LEFT)
				target = topTarget;
			else
				target = depthTarget;
			break;
		case GFX_BOTTOM:
			target = bottomTarget;
			break;
	}

	return target;
}

void Graphics::Clear(gfxScreen_t screen, gfx3dSide_t side)
{
	C2D_TargetClear(GetScreen(screen, side), backgroundColor);
}

void Graphics::DrawOn(gfxScreen_t screen, gfx3dSide_t side)
{
	renderScreen = screen;
	currentSide = side;

	C2D_SceneBegin(GetScreen(screen, side));
}

void Graphics::Exit()
{
	C2D_Fini();
	C3D_Fini();
	gfxExit();
}

//Register Functions
int Graphics::Register(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "newImage",			imageNew			},
		{ "newQuad",			quadNew				},
		{ "newFont",			fontNew				},
		{ "getWidth",			GetWidth			},
		{ "getHeight",			GetHeight			},
		{ "draw",				Draw				},
		{ "print",				Print				},
		{ "rectangle",			Rectangle			},
		{ "set3D",				Set3D				},
		{ "setDepth",			SetDepth			},
		{ "setScissor",			SetScissor			},
		{ "circle",				Circle				},
		{ "setDefaultFilter",	SetDefaultFilter	},
		{ "getRendererInfo",	GetRendererInfo		},
		{ "setFont",			SetFont				},
		{ "setScreen",			SetScreen			},
		{ "setBackgroundColor", SetBackgroundColor	},
		{ "setColor",			SetColor 			},
		{ "push",				Push				},
		{ "pop",				Pop					},
		{ "translate",			Translate 			},
		{ "shear",				Shear	 			},
		{ "scale",				Scale	 			},
		{ "rotate",				Rotate	 			},
		{ "origin",				Origin	 			},
		{ "clear",				Clear				},
		{ "present",			Present 			},
		{ 0, 0 }
	};

	luaL_newlib(L, reg);
	
	return 1;
}