#include "common/runtime.h"
#include "modules/graphics.h"

#include "common/drawable.h"
#include "objects/image/wrap_image.h"
#include "objects/image/image.h"

#include "objects/quad/wrap_quad.h"
#include "objects/quad/quad.h"

#include "objects/font/wrap_font.h"
#include "objects/font/font.h"

#include "objects/spritebatch/wrap_spritebatch.h"
#include "objects/spritebatch/spritebatch.h"

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
TextureFilter defaultFilter;

vector<StackMatrix> transformStack(16);
bool STACK_PUSHED = false;

static void Transform(float * originalX, float * originalY, float * originalRotation, float * originalScalarX, float * originalScalarY) // rotate, scale, and translate coords.
{
    if (transformStack.empty())
        return;

    StackMatrix matrix = transformStack.back();

    float newLeft = *originalX;
    float newTop = *originalY;

    //Translate
    *originalX += matrix.ox;
    *originalY += matrix.oy;

    //Scale
    *originalX *= matrix.sx;
    *originalY *= matrix.sy;

    *originalScalarX *= matrix.sx;
    *originalScalarY *= matrix.sy;
    
    //Rotate
    if (*originalRotation != 0)
    {
        *originalX = newLeft * cos(matrix.r) - newTop * sin(matrix.r);
        *originalY = newLeft * sin(matrix.r) + newTop * cos(matrix.r);

        *originalRotation += matrix.r;
    }

    //Shear
    //*originalX = newLeft + matrix.kx * newTop;
    //*originalY = newTop + matrix.ky * newLeft;
}

void Graphics::Initialize()
{
    gfxInitDefault();

    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    
    topTarget = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    bottomTarget = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    depthTarget = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);

    StackMatrix init = {0, 0, 0, 1, 1, 0, 0};
    transformStack.emplace_back(init);

    defaultFilter.minFilter = GPU_LINEAR;
    defaultFilter.magFilter = GPU_LINEAR;
    
    STACK_PUSHED = false;
}

//Löve2D Functions

//love.graphics.rectangle
int Graphics::Rectangle(lua_State * L)
{
    string mode = luaL_checkstring(L, 1);

    float x = luaL_optnumber(L, 2, 0);
    float y = luaL_optnumber(L, 3, 0);
    float width = luaL_checknumber(L, 4);
    float height = luaL_checknumber(L, 5);

    Transform(&x, &y, 0, 0, 0);

    if (currentScreen == renderScreen)
    {
        if (mode == "fill")
            C2D_DrawRectSolid(x, y, 0.5, width, height, ConvertColor(drawColor));
        else if (mode == "line")
        {
            C2D_DrawRectSolid(x, y, 0.5, 1, height, ConvertColor(drawColor)); // tl -> bl
            C2D_DrawRectSolid(x + 1, y, 0.5, width - 1, 1, ConvertColor(drawColor)); // tl -> tr
            C2D_DrawRectSolid(x + width, y, 0.5, 1, height, ConvertColor(drawColor)); // tr -> br
            C2D_DrawRectSolid(x + 1, y + height, 0.5, width - 1, 1, ConvertColor(drawColor)); // bl -> br
        }
    }
    
    return 0;
}

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

    Transform(&x, &y, &rotation, &scalarX, &scalarY);

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

    const char * text = luaL_checkstring(L, 1);

    float x = luaL_optnumber(L, 2, 0);
    float y = luaL_optnumber(L, 3, 0);

    float rotation = luaL_optnumber(L, 4, 0);

    float scalarX = luaL_optnumber(L, 5, 1);
    float scalarY = luaL_optnumber(L, 6, 1);

    Transform(&x, &y, &rotation, &scalarX, &scalarY);

    if (currentScreen == renderScreen)
        currentFont->Print(text, x, y, rotation, scalarX, scalarY, drawColor);

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
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    const char * screen = luaL_optstring(L, 1, "top");

    if (strncmp(screen, "top", 3) == 0)
        Clear(GFX_TOP, GFX_LEFT);
    else
        Clear(GFX_BOTTOM, GFX_LEFT);

    return 0;
}

int Graphics::SetDefaultFilter(lua_State * L)
{
    string minFilter = luaL_checkstring(L, 1);
    string magFilter = luaL_checkstring(L, 2);

    GPU_TEXTURE_FILTER_PARAM min = GPU_LINEAR;
    if (minFilter == "nearest")
        min = GPU_NEAREST;

    GPU_TEXTURE_FILTER_PARAM mag = GPU_LINEAR;
    if (magFilter == "nearest")
        mag = GPU_NEAREST;

    defaultFilter.minFilter = min;
    defaultFilter.magFilter = mag;
}

//love.graphics.setScissor
int Graphics::SetScissor(lua_State * L)
{
    GPU_SCISSORMODE mode = (lua_gettop(L) != 0) ? GPU_SCISSOR_NORMAL : GPU_SCISSOR_DISABLE;

    float x      = luaL_optnumber(L, 1, 0);
    float y      = luaL_optnumber(L, 2, 0);
    float width  = luaL_optnumber(L, 3, 0);
    float height = luaL_optnumber(L, 4, 0);

    if (width < 0 || height < 0)
        luaL_error(L, "Scissor cannot have negative width or height.");

    float screenWidth = 400;
    if (currentScreen == GFX_BOTTOM)
        screenWidth = 320;

    if (currentScreen == renderScreen)
        C3D_SetScissor(mode, 240 - (y + height), screenWidth - (x + width), 240 - y, screenWidth - x);

    return 0;
}

//love.graphics.present
int Graphics::Present(lua_State * L)
{
    C3D_FrameEnd(0);

    return 0;
}

//love.graphics.push
int Graphics::Push(lua_State * L)
{
    transformStack.push_back(transformStack.back());

    return 0;
}

//love.graphics.pop
int Graphics::Pop(lua_State * L)
{
    transformStack.pop_back();

    return 0;
}

//love.graphics.translate
int Graphics::Translate(lua_State * L)
{
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);

    if (currentScreen == renderScreen)
    {
        transformStack.back().ox += x;
        transformStack.back().oy += y;
    }

    return 0;
}

//love.graphics.scale
int Graphics::Scale(lua_State * L)
{
    float scalarX = luaL_checknumber(L, 1);
    float scalarY = luaL_checknumber(L, 2);

    if (currentScreen == renderScreen)
    {
        transformStack.back().sx = scalarX;
        transformStack.back().sy = scalarY;
    }

    return 0;
}

//love.graphics.rotate
int Graphics::Rotate(lua_State * L)
{
    float rotation = luaL_checknumber(L, 1);

    if (currentScreen == renderScreen)
        transformStack.back().r = rotation;

    return 0;
}

//love.graphics.origin
int Graphics::Origin(lua_State * L)
{
    if (currentScreen == renderScreen)
    {
        transformStack.back().ox = 0;
        transformStack.back().oy = 0;

        transformStack.back().sx = 1;
        transformStack.back().sy = 1;

        transformStack.back().r = 0;

        transformStack.back().kx = 0;
        transformStack.back().ky = 0;
    }

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

TextureFilter Graphics::GetFilter()
{
    return defaultFilter;
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
        { "newSpriteBatch",     spriteBatchNew     },
        { "newFont",            fontNew            },
        { "getWidth",           GetWidth           },
        { "getHeight",          GetHeight          },
        { "draw",               Draw               },
        { "print",              Print              },
        { "rectangle",          Rectangle          },
        //{ "set3D",              Set3D              },
        //{ "setDepth",           SetDepth           },
        { "setScissor",         SetScissor         },
        //{ "circle",             Circle             }
        { "setDefaultFilter",   SetDefaultFilter   },
        { "getRendererInfo",    GetRendererInfo    },
        { "setFont",            SetFont            },
        { "setScreen",          SetScreen          },
        { "setBackgroundColor", SetBackgroundColor },
        { "setColor",           SetColor           },
        { "push",             Push               },
        { "pop",                Pop                },
        { "translate",          Translate          },
        //{ "shear",              Shear              },
        { "scale",              Scale              },
        { "rotate",             Rotate             },
        { "origin",             Origin             },
        { "clear",              Clear              },
        { "present",            Present            },
        { 0, 0 }
    };

    luaL_newlib(L, reg);
    
    return 1;
}