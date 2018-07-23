#include "common/runtime.h"

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

#include "objects/text/text.h"
#include "objects/text/wrap_text.h"

bool isInitialized = false;
SDL_Color backgroundColor = { 0, 0, 0, 255 };
SDL_Color drawColor = { 255, 255, 255, 255 };

float scaleX = 1;
float scaleY = 1;

Font * currentFont = NULL;

float Graphics::GetXScale()
{
	return scaleX;
}

float Graphics::GetYScale()
{
	return scaleY;
}

vector<StackMatrix> stack;
void transformDrawable(float * originalX, float * originalY) // rotate, scale, and translate coords.
{
    if (stack.empty())
        return;

    StackMatrix & matrix = stack.back();

    float newLeft = *originalX;
    float newTop = *originalY;

    //Translate
    *originalX += matrix.ox;
    *originalY += matrix.oy;

    //Scale
    /*originalX *= matrix.sx;
    *originalY *= matrix.sy;

    //Rotate
    *originalX = newLeft * cos(matrix.r) - newTop * sin(matrix.r);
    *originalY = newLeft * sin(matrix.r) + newTop * cos(matrix.r);

    //Shear
    *originalX = newLeft + matrix.kx * newTop;
    *originalY = newTop + matrix.ky * newLeft;*/
}

void Graphics::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        printf("Failed to load SDL2");

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

    backgroundColor.r = r * 255;
    backgroundColor.g = g * 255;
    backgroundColor.b = b * 255;

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
            drawable = (Text *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_TEXT);

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

    float x = luaL_optnumber(L, start + 0, 0);
    float y = luaL_optnumber(L, start + 1, 0);

    float rotation = luaL_optnumber(L, start + 2, 0);

    float scalarX = luaL_optnumber(L, start + 3, 1);
    float scalarY = luaL_optnumber(L, start + 4, 1);

    float offsetX = luaL_optnumber(L, start + 5, 0);
    float offsetY = luaL_optnumber(L, start + 6, 0);

    SDL_Rect quadRectangle;

    transformDrawable(&x, &y);
    x -= offsetX;
    y -= offsetY;

    if (quad != nullptr)
    {
        quadRectangle = {quad->GetX(), quad->GetY(), quad->GetWidth(), quad->GetHeight()};
        drawable->Draw(&quadRectangle, x * scaleX, y * scaleY, rotation, scalarX * scaleX, scalarY * scaleY, drawColor);
    }
    else
        drawable->Draw(NULL, x * scaleX, y * scaleY, rotation, scalarX * scaleX, scalarY * scaleY, drawColor);

    return 0;
}

int Graphics::SetDefaultFilter(lua_State * L)
{
    return 0;
}

//love.graphics.getFont
int Graphics::GetFont(lua_State * L)
{
	love_push_userdata(L, currentFont);

    return 1;
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
    const char * text = luaL_checkstring(L, 1);

    float x = luaL_optnumber(L, 2, 0);
    float y = luaL_optnumber(L, 3, 0);

    transformDrawable(&x, &y);

    if (currentFont == NULL)
        return 0;

    currentFont->Print(text, x, y, drawColor);

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
    string mode = luaL_checkstring(L, 1);

    LOVE_VALIDATE_DRAW_MODE(mode.c_str());

    float x = luaL_optnumber(L, 2, 0);
    float y = luaL_optnumber(L, 3, 0);

    float width = luaL_checknumber(L, 4);
    float height = luaL_checknumber(L, 5);

    transformDrawable(&x, &y);

    if (mode == "fill")
        boxRGBA(Window::GetRenderer(), x * scaleX, y * scaleY, ((x + width) * scaleX) - 1, ((y + height) * scaleY) - 1, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
    else if (mode == "line")
        rectangleRGBA(Window::GetRenderer(), x * scaleX, y * scaleY, ((x + width) * scaleX) - 1, ((y + height) * scaleY) - 1, drawColor.r, drawColor.g, drawColor.b, drawColor.a);

    return 0;
}

//love.graphics.polygon
int Graphics::Polygon(lua_State * L)
{
    string mode = luaL_checkstring(L, 1);

    LOVE_VALIDATE_DRAW_MODE(mode.c_str());

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float bx = luaL_checknumber(L, 4);
    float by = luaL_checknumber(L, 5);
	
	float cx = luaL_checknumber(L, 6);
    float cy = luaL_checknumber(L, 7);

    transformDrawable(&x, &y);

    if (mode == "fill")
        filledTrigonRGBA(Window::GetRenderer(), x * scaleX, y * scaleY, bx * scaleX, by * scaleY, cx * scaleX, cy * scaleY, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
    else if (mode == "line")
        trigonRGBA(Window::GetRenderer(), x * scaleX, y * scaleY, bx * scaleX, by * scaleY, cx * scaleX, cy * scaleY, drawColor.r, drawColor.g, drawColor.b, drawColor.a);

    return 0;
}

//love.graphics.arc
int Graphics::Arc(lua_State * L)
{
    string mode = luaL_checkstring(L, 1);

    LOVE_VALIDATE_DRAW_MODE(mode.c_str());

    float x = luaL_optnumber(L, 2, 0);
    float y = luaL_optnumber(L, 3, 0);

    float radius = luaL_checknumber(L, 4);

    float start = luaL_checknumber(L, 5);
    float end = luaL_checknumber(L, 6);

    start *= 180 / M_PI;
    end *= 180 / M_PI;

    if (mode == "line")
        pieRGBA(Window::GetRenderer(), x, y, radius, start, end, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
    else if (mode == "fill")
        filledPieRGBA(Window::GetRenderer(), x, y, radius, start, end, drawColor.r, drawColor.g, drawColor.b, drawColor.a);

    return 0;
}

//love.graphics.circle
int Graphics::Circle(lua_State * L)
{
    string mode = luaL_checkstring(L, 1);

    LOVE_VALIDATE_DRAW_MODE(mode.c_str());

    float x = luaL_optnumber(L, 2, 0);
    float y = luaL_optnumber(L, 3, 0);

    float radius = luaL_checknumber(L, 4);

    if (mode == "fill")
        filledCircleRGBA(Window::GetRenderer(), x * scaleX, y * scaleY, radius * scaleX, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
    else if (mode == "line")
        circleRGBA(Window::GetRenderer(), x * scaleX, y * scaleY, radius * scaleX, drawColor.r, drawColor.g, drawColor.b, drawColor.a);

    return 0;
}

//love.graphics.line
int Graphics::Line(lua_State * L)
{
    int argc = lua_gettop(L);
    bool isTable = lua_istable(L, 1);
    int tableLength = 0;

    float startx = 0, starty = 0, endx = 0, endy = 0;

    if (isTable)
    {
        tableLength = lua_objlen(L, 1);

        if (tableLength == 0 || (tableLength > 0 && (tableLength % 4) != 0))
            return luaL_error(L, "%s", "Need at least two verticies to draw a line");

        if ((tableLength % 4) == 0)
        {
            for (int j = 0; j < tableLength; j += 4)
            {
                for (int i = 1; i <= 4; i++)
                    lua_rawgeti(L, 1, i + j);

                startx = luaL_checknumber(L, -4);
                starty = luaL_checknumber(L, -3);

                endx = luaL_checknumber(L, -2);
                endy = luaL_checknumber(L, -1);

                lua_pop(L, 4);

                lineRGBA(Window::GetRenderer(), startx * scaleX, starty * scaleY, endx * scaleX, endy * scaleY, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
            }
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

            lineRGBA(Window::GetRenderer(), startx * scaleX, starty * scaleY, endx * scaleX, endy * scaleY, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
        }
    }

    return 0;
}

//love.graphics.points
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
        return luaL_error(L, "Points must be a multiple of two");

    int pointCount = args / 2;
    if (tableOfTables)
        pointCount = args;

    float * coordinates = new float[pointCount * 2];
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

    SDL_Point points[pointCount];
    for (int i = 0; i < args; i++)
    {
        points[i].x = coordinates[i * 2 + 0];
        points[i].y = coordinates[i * 2 + 1];
    }

    SDL_RenderDrawPoints(Window::GetRenderer(), points, args);

    delete[] coordinates;

    return 0;
}

//love.graphics.setScissor
int Graphics::SetScissor(lua_State * L)
{
    if (lua_isnoneornil(L, 1))
        SDL_RenderSetClipRect(Window::GetRenderer(), NULL);
    else
    {
        int x = luaL_checknumber(L, 1);
        int y = luaL_checknumber(L, 2);

        int width = luaL_checknumber(L, 3);
        int height = luaL_checknumber(L, 4);

        if (width < 0 || height < 0)
            return luaL_error(L, "Scissor cannot have a negative width or height");

        SDL_Rect scissor = {x, y, width, height};

        SDL_RenderSetClipRect(Window::GetRenderer(), &scissor);
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
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);

    stack.back().ox += x;
    stack.back().oy += y;

    return 0;
}

//love.graphics.scale
int Graphics::Scale(lua_State * L)
{
    float sx = luaL_checknumber(L, 1);
    float sy = luaL_checknumber(L, 2);

    stack.back().sx = sx;
    stack.back().sy = sy;
	
	scaleX = sx;
	scaleY = sy;

    return 0;
}

//love.graphics.rotate
int Graphics::Rotate(lua_State * L)
{
    float r = luaL_checknumber(L, 1);

    stack.back().r = r;

    return 0;
}

//love.graphics.shear
int Graphics::Shear(lua_State * L)
{
    float kx = luaL_checknumber(L, 1);
    float ky = luaL_checknumber(L, 2);

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
        { "arc",                Arc                },
        { "circle",             Circle             },
        { "clear",              Clear              },
        { "draw",               Draw               },
        { "getBackgroundColor", GetBackgroundColor },
        { "getFont",            GetFont            },
        { "getHeight",          GetHeight          },
        { "getRendererInfo",    GetRendererInfo    },
        { "getWidth",           GetWidth           },
        { "line",               Line               },
        { "newCanvas",          canvasNew          },
        { "newFont",            fontNew            },
        { "newImage",           imageNew           },
        { "newQuad",            quadNew            },
        { "origin",             Origin             },
        { "points",             Points             },
        { "pop",                Pop                },
        { "present",            Present            },
        { "print",              Print              },
        { "push",               Push               },
        { "rectangle",          Rectangle          },
        { "polygon",            Polygon            },
        { "rotate",             Rotate             },
        { "scale",              Scale              },
        { "setBackgroundColor", SetBackgroundColor },
        { "setCanvas",          SetCanvas          },
        { "setColor",           SetColor           },
        { "setDefaultFilter",   SetDefaultFilter   },
        { "setFont",            SetFont            },
        { "setNewFont",         SetNewFont         },
        { "setScissor",         SetScissor         },
        { "shear",              Shear              },
        { "translate",          Translate          },
        { 0, 0 }
    };

    luaL_newlib(L, reg);

    return 1;
}