#include "common/runtime.h"
#include "modules/graphics.h"

#include "common/drawable.h"
#include "objects/image/wrap_image.h"
#include "objects/image/image.h"

#include "objects/quad/wrap_quad.h"
#include "objects/quad/quad.h"

#include "objects/font/wrap_font.h"
#include "objects/font/font.h"

bool GFX_INIT = false;

//Screen Stuff
gfxScreen_t currentScreen = GFX_TOP;
gfx3dSide_t currentSide = GFX_LEFT;
gfxScreen_t renderScreen;

//Rendertargets
C3D_RenderTarget * topTarget = nullptr;
C3D_RenderTarget * depthTarget = nullptr;
C3D_RenderTarget * bottomTarget = nullptr;

Color backgroundColor = { 0, 0, 0, 1 };
Color drawColor = { 1, 1, 1, 1 };

Font * currentFont = nullptr;

void Graphics::Initialize()
{
    gfxInitDefault();

    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    
    topTarget = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    bottomTarget = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    depthTarget = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);

    GFX_INIT = true;
}

bool Graphics::IsInitialized()
{
    return GFX_INIT;
}

//Löve2D Functions

//love.graphics.draw
int Graphics::Draw(lua_State * L)
{
    Drawable * drawable = (Image *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGE);

    Quad * quad = nullptr;
    Tex3DS_SubTexture subTexture;

    int start = 2;
    if (!lua_isnoneornil(L, 2) && !lua_isnumber(L, 2))
    {
        quad = (Quad *)luaobj_checkudata(L, 2, LUAOBJ_TYPE_QUAD);
        
        subTexture = drawable->GetSubTexture();
        
        quad->SetSubTexture(&subTexture);
        drawable->SetSubTexture(subTexture);

        start = 3;
    }

    float x = luaL_optnumber(L, start + 0, 0);
    float y = luaL_optnumber(L, start + 1, 0);

    float rotation = luaL_optnumber(L, start + 2, 0);

    float scalarX = luaL_optnumber(L, start + 3, 1);
    float scalarY = luaL_optnumber(L, start + 4, 1);

    float offsetX = luaL_optnumber(L, start + 5, 0);
    float offsetY = luaL_optnumber(L, start + 6, 0);

    //transformDrawable(&x, &y);
    x -= offsetX;
    y -= offsetY;

    if (currentScreen == renderScreen)
        drawable->Draw(x, y, rotation, scalarX, scalarY, drawColor);

    return 0;
}

//love.graphics.print
int Graphics::Print(lua_State * L)
{
    if (currentFont == nullptr)
        return 0;

    size_t length;
    const char * text = luaL_checklstring(L, 1, &length);

    float x = luaL_optnumber(L, 2, 0);
    float y = luaL_optnumber(L, 3, 0);

    if (currentScreen == renderScreen)
        currentFont->Print(text, length, x, y, drawColor);

    return 0;
}

//love.graphics.setScreen
int Graphics::SetScreen(lua_State * L)
{
	string screen = luaL_checkstring(L, 1);

	gfxScreen_t switchScreen = GFX_TOP;
	if (screen == "bottom")
		switchScreen = GFX_BOTTOM;

	currentScreen = switchScreen;

	return 0;
}

//love.graphics.setFont
int Graphics::SetFont(lua_State * L)
{
    Font * self = (Font *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_FONT);

    if (self != nullptr)
        currentFont = self;

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

//love.graphics.getRendererInfo
int Graphics::GetRendererInfo(lua_State * L)
{
    lua_pushstring(L, "OpenGL ES");
    lua_pushstring(L, "1.1");
    lua_pushstring(L, "Digital Media Professionals Inc.");
    lua_pushstring(L, "DMP PICA200");
 
    return 4;
}

//love.graphics.setBackgroundColor
int Graphics::SetBackgroundColor(lua_State * L)
{
    float r = 0, g = 0, b = 0;

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

    backgroundColor.r = r;
    backgroundColor.g = g;
    backgroundColor.b = b;

    return 0;
}

//love.graphics.setColor
int Graphics::SetColor(lua_State * L)
{
    float r = 0, g = 0, b = 0, a = 1;

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

    drawColor.r = r;
    drawColor.g = g;
    drawColor.b = b;
    drawColor.a = a;

    return 0;
}

//love.graphics.clear
int Graphics::Clear(lua_State * L)
{
    C2D_TargetClear(GetScreen(GFX_TOP, GFX_LEFT), ConvertColor(backgroundColor));
    C2D_TargetClear(GetScreen(GFX_TOP, GFX_RIGHT), ConvertColor(backgroundColor));
    C2D_TargetClear(GetScreen(GFX_BOTTOM, GFX_LEFT), ConvertColor(backgroundColor));

    return 0;
}

//love.graphics.present
int Graphics::Present(lua_State * L)
{
    C3D_FrameEnd(0);

    return 0;
}

//End Löve2D Functions
u32 Graphics::ConvertColor(Color & color)
{
    return C2D_Color32f(color.r, color.g, color.b, color.a);
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
    C2D_TargetClear(GetScreen(screen, side), ConvertColor(backgroundColor));
    DrawOn(screen, side);
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
        { "newImage",           imageNew           },
        { "newQuad",            quadNew            },
        { "newFont",            fontNew            },
        { "getWidth",           GetWidth           },
        { "getHeight",          GetHeight          },
        { "draw",               Draw               },
        { "print",              Print              },
        /*{ "rectangle",          Rectangle          },
        { "set3D",              Set3D              },
        { "setDepth",           SetDepth           },
        { "setScissor",         SetScissor         },
        { "circle",             Circle             },
        //{ "setDefaultFilter",   SetDefaultFilter   },*/
        { "getRendererInfo",    GetRendererInfo    },
        { "setFont",            SetFont            },
        { "setScreen",          SetScreen          },
        { "setBackgroundColor", SetBackgroundColor },
        { "setColor",           SetColor           },
        /*{ "push",             Push               },
        { "pop",                Pop                },
        { "translate",          Translate          },
        { "shear",              Shear              },
        { "scale",              Scale              },
        { "rotate",             Rotate             },
        { "origin",             Origin             },*/
        { "clear",              Clear              },
        { "present",            Present            },
        { 0, 0 }
    };

    luaL_newlib(L, reg);
    
    return 1;
}