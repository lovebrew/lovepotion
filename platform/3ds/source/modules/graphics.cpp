#include "common/runtime.h"
#include "modules/graphics.h"
#include "modules/display.h"

#include "common/drawable.h"
#include "objects/image/wrap_image.h"
#include "objects/image/image.h"

#include "objects/quad/wrap_quad.h"
#include "objects/quad/quad.h"

#include "objects/font/wrap_font.h"
#include "objects/font/font.h"


Font * currentFont = nullptr;

vector<StackMatrix> transformStack;
bool STACK_PUSHED = false;

static void Transform(float * originalX, float * originalY, float * originalRotation, float * originalScalarX, float * originalScalarY) // rotate, scale, and translate coords.
{
    if (transformStack.empty())
        return;

    StackMatrix matrix = transformStack.back();

    float newLeft = *originalX;
    float newTop = *originalY;

    float slider = osGet3DSliderState();
    /*if (gfxIs3D() && currentScreen == GFX_TOP)
    {
        if (currentSide == GFX_LEFT)
            *originalX -= (slider * currentDepth);
        else if (currentSide == GFX_RIGHT)
            *originalX += (slider * currentDepth);
    }*/

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

    transformStack.reserve(16);

    StackMatrix stack = {0, 0, 0, 1, 1, 0, 0};
    transformStack.emplace_back(stack);

    STACK_PUSHED = false;
}

//Löve2D Functions

//love.graphics.set3D
int Graphics::Set3D(lua_State * L)
{
    bool enable = lua_toboolean(L, 1);

    gfxSet3D(enable);

    lua_pushboolean(L, gfxIs3D());

    return 1;
}

int Graphics::SetDepth(lua_State * L)
{
    depth = luaL_checknumber(L, 1);

    return 0;
}

//love.graphics.rectangle
int Graphics::Rectangle(lua_State * L)
{
    string mode = luaL_checkstring(L, 1);

    float x = luaL_optnumber(L, 2, 0);
    float y = luaL_optnumber(L, 3, 0);
    float width = luaL_checknumber(L, 4);
    float height = luaL_checknumber(L, 5);

    float rotation = 0;
    float scalarX = 0;
    float scalarY = 0;

    Transform(&x, &y, &rotation, &scalarX, &scalarY);

    if (Display::IsRenderingScreen(screen))
    {
        if (mode == "fill")
            C2D_DrawRectSolid(x, y, 0.5, width, height, ConvertColor(blendColor));
        else if (mode == "line")
        {
            C2D_DrawRectSolid(x, y, 0.5, 1, height, ConvertColor(blendColor)); // tl -> bl
            C2D_DrawRectSolid(x + 1, y, 0.5, width - 1, 1, ConvertColor(blendColor)); // tl -> tr
            C2D_DrawRectSolid(x + width, y, 0.5, 1, height, ConvertColor(blendColor)); // tr -> br
            C2D_DrawRectSolid(x + 1, y + height, 0.5, width - 1, 1, ConvertColor(blendColor)); // bl -> br
        }
    }
    
    return 0;
}

//love.graphics.draw
int Graphics::Draw(lua_State * L)
{
    Drawable * drawable = (Image *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGE);

    Quad * quad = nullptr;

    int start = 2;
    if (!lua_isnoneornil(L, 2) && !lua_isnumber(L, 2))
    {
        quad = (Quad *)luaobj_checkudata(L, 2, LUAOBJ_TYPE_QUAD);
        drawable->SetSubTexture(quad->GetSubTexture());

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

    if (Display::IsRenderingScreen(screen))
        drawable->Draw(x, y, rotation, scalarX, scalarY, blendColor);

    return 0;
}

int Graphics::Circle(lua_State * L)
{
    string mode = luaL_checkstring(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float radius = luaL_checknumber(L, 4);

    float rotation = 0;
    float scalarX = 0;
    float scalarY = 0;

    Transform(&x, &y, &rotation, &scalarX, &scalarY);

    if (Display::IsRenderingScreen(screen))
        C2D_DrawCircleSolid(x, y, 0.5, radius, ConvertColor(blendColor));

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

    if (Display::IsRenderingScreen(screen))
        currentFont->Print(text, x, y, rotation, scalarX, scalarY, blendColor);

    return 0;
}

//love.graphics.setScreen
int Graphics::SetScreen(lua_State * L)
{
    string newScreen = luaL_checkstring(L, 1);

    if (newScreen == "top")
        screen = GFX_TOP;
    else if (newScreen == "bottom")
        screen = GFX_BOTTOM;

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

//love.graphics.setNewFont
int Graphics::SetNewFont(lua_State *L)
{
    int ret = fontNew(L);
    Font *self = (Font *)luaobj_checkudata(L, -1, LUAOBJ_TYPE_FONT);
    currentFont = self;

    return ret;
}

//love.graphics.getDimensions
int Graphics::GetDimensions(lua_State * L)
{
    int width = 400;
    if (screen == GFX_BOTTOM)
        width = 320;

    lua_pushnumber(L, width);
    lua_pushnumber(L, 320);

    return 2;
}

//love.graphics.getWidth
int Graphics::GetWidth(lua_State * L)
{
    int width = 400;
    if (screen == GFX_BOTTOM)
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

int Graphics::SetDefaultFilter(lua_State * L)
{
    string min = luaL_checkstring(L, 1);
    string mag = luaL_checkstring(L, 2);

    if (min == "linear")
        filter[0] = GPU_LINEAR;
    else if (min == "nearest")
        filter[0] =  GPU_NEAREST;

    if (mag == "linear")
        filter[1] = GPU_LINEAR;
    else if (mag == "nearest")
        filter[1] =  GPU_NEAREST;

    return 0;
}

//love.graphics.setBackgroundColor
int Graphics::SetBackgroundColor(lua_State * L)
{
    float r = 0, g = 0, b = 0;

    if (lua_isnumber(L, 1))
    {
        r = clamp(0, luaL_checknumber(L, 1), 1);
        g = clamp(0, luaL_checknumber(L, 2), 1);
        b = clamp(0, luaL_checknumber(L, 3), 1);
    }
    else if (lua_istable(L, 1))
    {
        for (int i = 1; i <= 3; i++)
            lua_rawgeti(L, 1, i);

        r = clamp(0, luaL_checknumber(L, -3), 1);
        g = clamp(0, luaL_checknumber(L, -2), 1);
        b = clamp(0, luaL_checknumber(L, -1), 1);
    }

    backgColor.r = r;
    backgColor.g = g;
    backgColor.b = b;

    return 0;
}

//love.graphics.setColor
int Graphics::SetColor(lua_State * L)
{
    float r = 0, g = 0, b = 0, a = 1;

    if (lua_isnumber(L, 1))
    {
        r = clamp(0, luaL_checknumber(L, 1), 1);
        g = clamp(0, luaL_checknumber(L, 2), 1);
        b = clamp(0, luaL_checknumber(L, 3), 1);
        a = clamp(0, luaL_optnumber(L, 4, 1), 1);
    }
    else if (lua_istable(L, 1))
    {
        for (int i = 1; i <= 4; i++)
            lua_rawgeti(L, 1, i);

        r = clamp(0, luaL_checknumber(L, -4), 1);
        g = clamp(0, luaL_checknumber(L, -3), 1);
        b = clamp(0, luaL_checknumber(L, -2), 1);
        a = clamp(0, luaL_optnumber(L, -1, 1), 1);
    }

    blendColor.r = r;
    blendColor.g = g;
    blendColor.b = b;
    alpha = a;

    return 0;
}

//love.graphics.clear
int Graphics::Clear(lua_State * L)
{
    /*C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    const char * screen = luaL_optstring(L, 1, "top");

    if (strncmp(screen, "top", 3) == 0)
        Clear(GFX_TOP, GFX_LEFT);
    else
        Clear(GFX_BOTTOM, GFX_LEFT);
    */
    return 0;
}

//love.graphics.setScissor
int Graphics::SetScissor(lua_State * L)
{
    int args = lua_gettop(L);

    float x      = luaL_optnumber(L, 1, 0);
    float y      = luaL_optnumber(L, 2, 0);
    float width  = luaL_optnumber(L, 3, 0);
    float height = luaL_optnumber(L, 4, 0);

    if (Display::IsRenderingScreen(screen))
    {
        if (width < 0 || height < 0)
            luaL_error(L, "Scissor cannot have negative width or height.");

        GPU_SCISSORMODE mode = GPU_SCISSOR_NORMAL;
        if (args == 0) 
            mode = GPU_SCISSOR_DISABLE;

        float screenWidth = 400.0f;
        if (screen == GFX_BOTTOM)
            screenWidth = 320.0f;

        C2D_Flush();
        C3D_SetScissor(mode, 240 - (y + height), screenWidth - (x + width), 240 - y, screenWidth - x);
    }

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

    if (Display::IsRenderingScreen(screen))
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

    if (Display::IsRenderingScreen(screen))
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

    if (Display::IsRenderingScreen(screen))
        transformStack.back().r = rotation;

    return 0;
}

//love.graphics.origin
int Graphics::Origin(lua_State * L)
{
    if (Display::IsRenderingScreen(screen))
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

// Helper functions

std::array<GPU_TEXTURE_FILTER_PARAM, 2> Graphics::GetFilters()
{
    return filter;
}

Color Graphics::GetBackgroundColor()
{
    return backgColor;
}

u32 Graphics::ConvertColor(Color & color)
{
    return C2D_Color32f(color.r, color.g, color.b, color.a);
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
        { "circle",             Circle             },
        { "clear",              Clear              },
        { "draw",               Draw               },
        { "getDimensions",      GetDimensions      },
        { "getHeight",          GetHeight          },
        { "getRendererInfo",    GetRendererInfo    },
        { "getWidth",           GetWidth           },
        { "newFont",            fontNew            },
        { "newImage",           imageNew           },
        { "newQuad",            quadNew            },
        { "origin",             Origin             },
        { "pop",                Pop                },
        { "present",            Present            },
        { "print",              Print              },
        { "push",               Push               },
        { "rectangle",          Rectangle          },
        { "rotate",             Rotate             },
        { "scale",              Scale              },
        { "set3D",              Set3D              },
        { "setBackgroundColor", SetBackgroundColor },
        { "setColor",           SetColor           },
        { "setDefaultFilter",   SetDefaultFilter   },
        { "setDepth",           SetDepth           },
        { "setFont",            SetFont            },
        { "setNewFont",         SetNewFont         },
        { "setScissor",         SetScissor         },
        { "setScreen",          SetScreen          },
        { "translate",          Translate          },
        //{ "shear",              Shear              },
        { 0, 0 }
    };

    luaL_newlib(L, reg);
    
    return 1;
}
